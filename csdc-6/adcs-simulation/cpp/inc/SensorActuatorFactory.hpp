/** 
 * @file Simulator.cpp
 *
 * @details header file for class to create the sensor and actuator objects
 *
 * @authors Lily de Loe
 *
 * Last Edited
 * 2022-10-24
 *
**/

#pragma once

#include "sim_interface.hpp"
#include "Simulator.hpp"
#include <memory>

class Simulator;

/**
 * @class SensorActuatorFactory
 *
 * @details factory class used in creating sensor and actuator objects
 *
 */
class SensorActuatorFactory {
public:
    /**
    * @name GetSensor
    * @param name [string], the name of the sensor
    *
    * @details getter for a sensor object from singleton class instance. Returns a unique
    * pointer of the type of sensor matching the configuration
    */
    static std::shared_ptr<Sensor> GetSensor(const std::string &name, Simulator* sim);

    /**
    * @name GetActuator
    * @param name [string], the name of the actuator
    *
    * @details getter for actuator object from singleton class instance. Returns a unique
    * pointer of the type of actuator matching the configuration
    */
    static std::shared_ptr<Actuator> GetActuator(const std::string &name, Simulator* sim);
};
