/** @file Simulator.cpp
 *
 *@details class file that would configure and propagate the simulation
 *
 *@authors Lily de Loe, Justin Paoli
 *
 *Last Edited
 *2022-11-04
 *
**/
#include <chrono>
#include <iostream>
#include "Simulator.hpp"
#include "ConfigurationSingleton.hpp"
#include "SensorActuatorFactory.hpp"

Simulator::Simulator(const std::string &configFile) {

    auto &config = Configuration::GetInstance();
    if (!config.Load(configFile)) {
        std::cout <<"Configuration Failed to load"<< std::endl;
    }

    for (const auto &sensor : config.GetSensorConfigs()) {
        //first is string, second is data (from map)
        this->create_sensor(sensor.first);
    }

    for (const auto &actuator : config.GetActuatorConfigs()) {
        //first is string, second is data (from map)
        this->create_actuator(actuator.first);
    }

    this->simulation_time = 0;
    this->last_called = -1;
}

//create sensor based on name
void Simulator::create_sensor(const std::string &name) {
    if (name.empty()) {
        std::cout <<"Device name must be populated. Got "
        << name << std::endl;
        return;
    }

    auto sensorPtr = SensorActuatorFactory::GetSensor(name);
    if (!sensorPtr) {
        std::cout << "Unknown sensor type: " << name << std::endl;
        return;
    }

    this->sensors[name] = std::move(sensorPtr);
}

//create actuator based on name
void Simulator::create_actuator(const std::string &name) {
    if (name.empty()) {
        std::cout << "Device name must be populated. Got "
        << name << std::endl;
        return;
    }

    auto actPtr = SensorActuatorFactory::GetActuator(name);
    if (!actPtr) {
        std::cout << "Unknown actuator type: " << name << std::endl;
        return;
    }

    this->actuators[name] = std::move(actPtr);
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

    this->update_adcs_devices();
}

void Simulator::timestep() {
    Eigen::Vector3f total_rw_torques = Eigen::Vector3f::Zero();
    for (const auto &a : actuators) {
        // Attempt to cast the actuator to a specific type, if it succeeds
        // we know it was an actuator of that type and can proceed. If it fails
        // dynamic_cast() will return nullptr and we proceed to the next case
        if (Reaction_wheel *rw = dynamic_cast<Reaction_wheel*>(a.second.get())) {
            actuator_state state = rw->get_current_state();
            Eigen::Vector3f rw_position = rw->get_postition();
            Eigen::Vector3f omega_i = state.velocity * rw_position;
            Eigen::Vector3f alpha_i = state.acceleration * rw_position;
            Eigen::Matrix3f inertia_i = rw->get_inertia_matrix();
            total_rw_torques -= inertia_i * alpha_i;
            total_rw_torques -= this->satellite->omega_b.cross(inertia_i * omega_i);
            delete rw;
            continue;
        }
    }

    Eigen::Matrix3f inertia_b_inverse = this->satellite->inertia_b.inverse();
    this->satellite->alpha_b = -inertia_b_inverse * 
        (this->satellite->omega_b.cross(this->satellite->inertia_b * this->satellite->omega_b));

    this->satellite->omega_b += this->satellite->alpha_b * (float) timestep_length;
    this->satellite->theta_b += this->satellite->omega_b * (float) timestep_length;
}

void Simulator::update_adcs_devices() {
    for (const auto &s : sensors) {
        // Similar typechecking/casting procedure as in the timestep() function
        if (Accelerometer *a = dynamic_cast<Accelerometer *>(s.second.get())) {
            Eigen::Vector3f r = a->get_positions()[0];
            vector<Eigen::VectorXf> new_vals = { this->satellite->alpha_b.cross(r) };
            s.second->set_current_vals(new_vals, this->simulation_time);
            delete a;
            continue;
        }
    }
}
