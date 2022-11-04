/** @file Simulator.cpp
 *
 *@details class file that would configure and propagate the simulation
 *
 *@authors Lily de Loe, Justin Paoli
 *
 *Last Edited
 *2022-11-03
 *
**/

#include <chrono>
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

    this->satellite = new Satellite {
        Eigen::Vector3f(),
        Eigen::Vector3f(),
        Eigen::Vector3f(),
        Eigen::Matrix3f(),
    };
}

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
    return time_passed
}

void Simulator::simulate(timestamp t) {
    for (; this->simulation_time <= this->simulation_time + t; this->simulation_time += this->timestep_length) {
        this->timestep();
    }

    this->update_adcs_devices();
}

void Simulator::timestep() {
    // TODO: Check what type each actuator is and process accordingly
    Eigen::Vector3f total_rw_torques = Eigen::Vector3f::Zero();
    for (const auto &a : actuators) {
        Eigen::Vector3f omega_i = a.second->sim_get_current_velocities();
        Eigen::Vector3f alpha_i = a.second->sim_get_current_accelerations();
        Eigen::Matrix3f inertia_i = a.second->sim_get_inertia_matrix();
        total_rw_torques -= inertia_i * alpha_i;
        total_rw_torques -= this->satellite->omega_b.cross(inertia_i * omega_i);
    }

    Eigen::Matrix3f inertia_b_inverse = this->satellite->inertia_b.inverse();
    this->satellite->alpha_b = -inertia_b_inverse * 
        (this->satellite->omega_b.cross(this->satellite->inertia_b * this->satellite->omega_b));

    this->satellite->omega_b += this->satellite->alpha_b * (float) timestep_length;
    this->satellite->theta_b += this->satellite->omega_b * (float) timestep_length;
}

void Simulator::update_adcs_devices() {
    for (const auto &s : sensors) {
        Eigen::Vector3f r = s.second->sim_get_position();
        s.second->sim_set_current_vals(this->satellite->alpha_b.cross(r));
    }
}
