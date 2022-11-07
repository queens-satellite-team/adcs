/** 
 * @file Simulator.cpp
 *
 * @details class file that would configure and propagate the simulation
 *
 * @authors Lily de Loe, Justin Paoli
 *
 * Last Edited
 * 2022-11-04
 *
**/

#include <chrono>
#include <iostream>

#include "ConfigurationSingleton.hpp"
#include "SensorActuatorFactory.hpp"
#include "PointingModeController.hpp"
#include "Simulator.hpp"

Simulator::Simulator() {
    this->simulation_time = 0;
    this->last_called = -1;
}

void Simulator::init(sim_config initial_values)
{
    /* TODO may need a check here */
    this->system_vals = initial_values;
}

timestamp Simulator::update_simulation() {
    timestamp time_passed = this->determine_time_passed();
    this->simulate(time_passed);
}

timestamp Simulator::set_adcs_sleep(timestamp duration) {
    timestamp time_passed = this->determine_time_passed();
    this->simulate(time_passed + duration);    
}

timestamp Simulator::determine_time_passed() {
    using namespace std::chrono;
    uint32_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    timestamp now = (timestamp) ms;
    timestamp time_passed = now - this->last_called;
    this->last_called = now;
    return time_passed;
}

void Simulator::simulate(timestamp t) {
    for (; this->simulation_time <= this->simulation_time + t; this->simulation_time += this->timestep_length) {
        this->timestep();
    }

    // this->update_adcs_devices();
}

void Simulator::timestep() {
    Eigen::Vector3f total_rw_torques = Eigen::Vector3f::Zero();
    for (sim_reaction_wheel wheel : system_vals.reaction_wheels) {
        // actuator_state state = rw->get_current_state();
        // Eigen::Vector3f rw_position = rw->get_position();
        // Eigen::Vector3f omega_i = state.velocity * rw_position;
        // Eigen::Vector3f alpha_i = state.acceleration * rw_position;
        // Eigen::Matrix3f inertia_i = rw->get_inertia_matrix();
        // total_rw_torques -= inertia_i * alpha_i;
        // total_rw_torques -= this->satellite->omega_b.cross(inertia_i * omega_i);
        total_rw_torques -= wheel.inertia * wheel.alpha;
        total_rw_torques -= system_vals.satellite.omega_b.cross(wheel.inertia * wheel.omega);
    }

    Eigen::Matrix3f inertia_b_inverse = system_vals.satellite.inertia_b.inverse();
    system_vals.satellite.alpha_b = -inertia_b_inverse * 
        (system_vals.satellite.omega_b.cross(system_vals.satellite.inertia_b * system_vals.satellite.omega_b));

    system_vals.satellite.omega_b += system_vals.satellite.alpha_b * (float) timestep_length;
    system_vals.satellite.theta_b += system_vals.satellite.omega_b * (float) timestep_length;

    // Update new internal sensor and actuator values

}

// void Simulator::update_adcs_devices() {
//     for (const auto &s : sensors) {
//         // Similar typechecking/casting procedure as in the timestep() function
//         if (Accelerometer *a = dynamic_cast<Accelerometer *>(s.second.get())) {
//             Eigen::Vector3f r = a->get_positions()[0];
//             vector<Eigen::VectorXf> new_vals = { this->satellite->alpha_b.cross(r) };
//             s.second->set_current_vals(new_vals, this->simulation_time);
//             continue;
//         }
//     }
// }

timestamp Simulator::reaction_wheel_update_desired_state(Eigen::Vector3f wheel_position, actuator_state new_target)
{
    // figure out what reaction wheel it is from the position vector

    // Update the target state (For now just change the acceleration to match,
    // when it reaches it's target position just change accel to 0)

    this->update_simulation();
    return this->simulation_time;
}

actuator_state Simulator::reaction_wheel_get_current_state(Eigen::Vector3f position)
{
    this->update_simulation();
    // figure out which reaction wheel it is
    uint8_t reaction_wheel_index = 0;
    actuator_state ret;

    // Do some math to convert body-frame values to reaction_wheel_frame
    ret.position     = 0; // this isn't used anyway
    ret.acceleration = 0;
    ret.velocity     = 0;
    ret.time         = this->simulation_time;

    return ret;
}

timestamp Simulator::gyroscope_take_measurement(Eigen::Vector3f *measurement)
{
    this->update_simulation();
    *measurement = this->system_vals.gyroscope.measurement;

    return this->simulation_time;
}

timestamp Simulator::accelerometer_take_measurement(Eigen::Vector3f *measurement)
{
    this->update_simulation();
    *measurement = this->system_vals.accelerometer.measurement;

    return this->simulation_time;
}