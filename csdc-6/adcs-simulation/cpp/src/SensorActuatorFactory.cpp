/** 
 * @file Simulator.cpp
 *
 * @details class file to create the sensor and actuator objects
 *
 * @authors Lily de Loe
 *
 * Last Edited
 * 2022-10-24
 *
**/

#include "sim_interface.hpp"
#include "SensorActuatorFactory.hpp"
#include "ConfigurationSingleton.hpp"

std::unique_ptr<Sensor> SensorActuatorFactory::GetSensor(const std::string &name) {
    auto &config = Configuration::GetInstance();
    const auto type = config.GetSensorConfig(name)->type;
    std::unique_ptr<Sensor> ret = {};

    switch(type){
    case SensorType::Accelerometer:
        ret = std::make_unique<Accelerometer>(name);
    case SensorType::Gyroscope:
        ret = std::make_unique<Gyroscope>(name);
    }
    return ret;
}

std::unique_ptr<Actuator> SensorActuatorFactory::GetActuator(const std::string &name) {
    auto &config = Configuration::GetInstance();
    const auto type = config.GetActuatorConfig(name)->type;
    std::unique_ptr<Actuator> ret = {};

    //this is set up to extend to more actuator types
    switch(type){
    case ActuatorType::Reaction_wheel:
        ret = std::make_unique<Reaction_wheel>(name);
    }
    return ret;
}
