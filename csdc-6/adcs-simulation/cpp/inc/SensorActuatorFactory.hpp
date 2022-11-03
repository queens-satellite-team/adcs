/** @file Simulator.cpp
 *
 *@details header file for class to create the sensor and actuator objects
 *
 *@authors Lily de Loe
 *
 *Last Edited
 *2022-10-24
 *
**/

#pragma once

#include "sim_interface.hpp"
#include <memory>

class SensorActuatorFactory {
public:
    static std::unique_ptr<Sensor> GetSensor(const std::string &name);
    static std::unique_ptr<Actuator> GetActuator(const std::string &name);
};
