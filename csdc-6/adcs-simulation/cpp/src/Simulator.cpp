#include <vector>
#include "Simulator.hpp"

Simulator::Simulator(vector<Sensor> sensors, vector<Actuator> actuators) {
    this->simulation_time = 0;
    this->sensors = sensors;
    this->actuators = actuators;
}

void Simulator::update_simulation() {

}

void Simulator::set_adcs_sleep(timestamp duration) {

}

timestamp Simulator::determine_time_passed() {

}

void Simulator::simulate(timestamp t) {

}

void Simulator::simulate(timestamp t) {
    
}

void Simulator::timestep() {

}

void Simulator::update_adcs_devices() {

}