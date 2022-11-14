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
    std::unordered_map<std::string, std::shared_ptr<Sensor>> sensors,
    std::unordered_map<std::string, std::shared_ptr<Actuator>> actuators,
    ADCS_timer *timer
) {
    this->sensors = sensors;
    this->actuators = actuators;
    this->timer = timer;
}

void PointingModeController::begin(std::vector<float> desired_attitude) {
    (void) desired_attitude;    /*  Silences "unsued parameter" compiler warning. This function will be
                                    updated later anyway so for now this is fine. */
    //TODO: convert to while and add exit condition
    for (int i = 0; i < 1; i++) {
        this->take_updated_measurements();
        //std::chrono::seconds duration(1);
        //std::this_thread::sleep_for(duration);
    }
}

void PointingModeController::take_updated_measurements() {
    for (const auto &s : sensors) {
        s.second->take_measurement();
    }
}