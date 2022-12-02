/**
 * @file Simulator.cpp
 *
 * @details implements the Simulator class as defined in Simulator.hpp
 *
 * @authors Lily de Loe, Justin Paoli, Aidan Sheedy
 *
 * Last Edited
 * 2022-12-01
 *
**/

#include <chrono>
#include <iostream>

#include "ConfigurationSingleton.hpp"
#include "SensorActuatorFactory.hpp"
#include "PointingModeController.hpp"
#include "Simulator.hpp"

Simulator::Simulator(Messenger *messenger)
{
    if (nullptr != messenger)
    {
        this->messenger = messenger;
    }

    this->simulation_time = 0;
    auto &config = Configuration::GetInstance();
    timestamp t(config.GetTimestepInMilliSeconds(),0);
    this->timestep_length = t;
    this->last_called = -1;

    this->variableTimestep = config.GetTimestepDecision();
    this->maxStep = config.GetMaxTimestep();
    this->minStep = config.GetMinTimestep();
}

void Simulator::init(sim_config initial_values, timestamp timeout)
{
    /* TODO may need a check here**/
    this->system_vals = initial_values;
    this->timeout = timeout;

    messenger->send_message("Starting simulation, timeout: " + this->timeout.pretty_string());
    messenger->start_new_sim(initial_values.reaction_wheels.size());
}

timestamp Simulator::update_simulation() {
    timestamp t(1, 0);
    this->simulate(t);

    return this->simulation_time;
}

timestamp Simulator::determine_timestep() {

    //2*max error is defined as 2*0.005 degrees = 0.01 degrees
    if (this->variableTimestep == true){
        float calculated_timestep = (0.01*M_PI/180)/(this->system_vals.satellite.alpha_b.cwiseAbs().maxCoeff())*1000;
        timestamp t(calculated_timestep,0);
        this->timestep_length = t;
        if (((float) timestep_length > maxStep/1000)) {
            timestamp t(20,0);
            this->timestep_length = t;
        }else if ((float) timestep_length < minStep/1000) {
            timestamp t(1,0);
            this->timestep_length = t;
        }
    }

    return timestep_length;

}

timestamp Simulator::set_adcs_sleep(timestamp duration) {
    timestamp t(1, 0);
    this->simulate(t + duration);

    return this->simulation_time;
}

timestamp Simulator::determine_time_passed() {
    if (this->last_called == -1) return 0;

    using namespace std::chrono;
    uint32_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    timestamp now = (timestamp) ms;
    timestamp time_passed = now - this->last_called;
    this->last_called = now;
    return time_passed;
}

void Simulator::simulate(timestamp t) {
    timestamp end = this->simulation_time + t;

    while (this->simulation_time <= end) {
        this->timestep_length = this->determine_timestep();
        this->simulation_time = this->simulation_time + this->timestep_length;
         
        this->timestep();
        this->messenger->update_simulation_state(this->system_vals, this->simulation_time);
        
        /* end simulation if the timeout is reached. */
        if (this->timeout < this->simulation_time)
        {
            this->messenger->close_open_csv();
            throw simulation_timeout("Timeout reached.");
        }
    }
}

void Simulator::timestep() {
    Eigen::Matrix3f inertia_b_inverse = system_vals.satellite.inertia_b.inverse();
    Eigen::Vector3f sum_rw = Eigen::Vector3f::Zero();

    for (sim_reaction_wheel &wheel : system_vals.reaction_wheels) {
        
        // This assumes that w_rw and I_rw are both scalars, and can thus be multiplied by the axis of rotation to achieve the right matrix dimensions
        // change this if either w_rw or I_rw become a matrix!
        sum_rw += (wheel.inertia * wheel.alpha * wheel.axis_of_rotation) 
            + system_vals.satellite.omega_b.cross(wheel.axis_of_rotation * wheel.omega * wheel.inertia);



        // Update reaction wheel velocity 
        wheel.omega += wheel.alpha * (float) this->timestep_length;
        //we need to consider alpha but this will be done by the controller
        //wheel.alpha +=  rw_jerk * (float) this->timestep_length;
    }
    
    system_vals.satellite.alpha_b = (-inertia_b_inverse * system_vals.satellite.omega_b).cross(system_vals.satellite.inertia_b * system_vals.satellite.omega_b)
        - inertia_b_inverse * sum_rw;

    system_vals.satellite.omega_b += system_vals.satellite.alpha_b * (float) timestep_length;
    system_vals.satellite.theta_b += system_vals.satellite.omega_b * (float) timestep_length;

    // Update new internal sensor and actuator values
    system_vals.accelerometer.measurement = system_vals.satellite.alpha_b.cross(system_vals.accelerometer.position);
    
    system_vals.gyroscope.alpha = system_vals.satellite.alpha_b;
    system_vals.gyroscope.omega = system_vals.satellite.omega_b;
    system_vals.gyroscope.theta = system_vals.satellite.theta_b;

    return;
}

timestamp Simulator::reaction_wheel_update_desired_state(Eigen::Vector3f wheel_position, actuator_state new_target)
{
    // figure out what reaction wheel it is from the position vector

    for (sim_reaction_wheel &wheel : system_vals.reaction_wheels) {
        if (wheel.position.isApprox(wheel_position)) {
            // Update the target state (For now just change the acceleration to match,
            // when it reaches it's target position just change accel to 0)
            wheel.alpha = new_target.acceleration;
        }
    }

    //this->update_simulation();
    return this->simulation_time;
}

actuator_state Simulator::reaction_wheel_get_current_state(Eigen::Vector3f position)
{
    this->update_simulation();
    actuator_state ret;
    // figure out which reaction wheel it is
    for (sim_reaction_wheel wheel : system_vals.reaction_wheels) {
        if (wheel.position.isApprox(position)) {
            // Do some math to convert body-frame values to reaction_wheel_frame
            // ret.position     = wheel.position; // this isn't used anyway
            ret.acceleration = wheel.alpha;
            ret.velocity     = wheel.omega;
            ret.time         = this->simulation_time;
        }
    }

    return ret;
}

gyro_state Simulator::gyroscope_take_measurement()
{
    this->update_simulation();
    gyro_state ret;

    ret.acceleration = this->system_vals.gyroscope.alpha;
    ret.velocity     = this->system_vals.gyroscope.omega;
    ret.position     = this->system_vals.gyroscope.theta;
    ret.time_taken   = this->simulation_time;

    return ret;
}

timestamp Simulator::accelerometer_take_measurement(Eigen::Vector3f *measurement)
{
    this->update_simulation();
    *measurement = this->system_vals.accelerometer.measurement;

    return this->simulation_time;
}
