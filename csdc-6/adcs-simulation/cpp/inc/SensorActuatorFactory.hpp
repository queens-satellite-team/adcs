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

#ifndef SENSORACTUATORFACTORY_HPP_
#define SENSORACTUATORFACTORY_HPP_

// #include "Sensor.hpp"
// #include "Actuator.hpp"
#include <memory>

class SensorActuatorFactory {
public:
    static std::unique_ptr<Sensor> GetSensor(const std::string &name);
    static std::unique_ptr<Actuator> GetActuator(const std::string &name);
}

#endif