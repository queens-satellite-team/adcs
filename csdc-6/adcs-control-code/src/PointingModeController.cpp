/** 
 * @file PointingModeController.cpp
 *
 * @details Implementation for the pointing mode controller
 *
 * @authors Justin Paoli
 *
 * Last Edited
 * 2022-11-06
**/

#include <chrono>
#include <thread>
#include "PointingModeController.hpp"

PointingModeController::PointingModeController(
    unordered_map<string, unique_ptr<Sensor>> sensors, 
    unordered_map<string, unique_ptr<Actuator>> actuators
) {
    this->sensors = sensors;
    this->actuators = actuators;
}

void PointingModeController::begin(vector<float> desired_attitude) {
    //TODO: convert to while and add exit condition
    for (int i = 0; i < 10; i++) {
        this->take_updated_measurements();
        std::chrono::seconds duration(1);
        std::this_thread::sleep_for(duration);
    }
}

void PointingModeController::take_updated_measurements() {
    for (const auto &s : sensors) {
        s.second->take_measurement();
    }
}