/** @file Simulator.cpp
 *
 *@details class file to create the sensor and actuator objects
 *
 *@authors Lily de Loe
 *
 *Last Edited
 *2022-10-24
 *
**/
#include "sim_interface.hpp"
#include "SensorActuatorFactory.hpp"
#include "ConfigurationSingleton.hpp"

//for type sensor
std::unique_ptr<Sensor>
SensorActuatorFactory::GetSensor(const std::string &name) {
    auto &config = Configuration::GetInstance();
    const auto type = config.GetSensorConfig(name)->type;
    switch(type){
    case SensorType::Accelerometer:
        return std::make_unique<Accelerometer>(name);
    case SensorType::Gyroscope:
        return std::make_unique<Gyroscope>(name);
    }
    return {};
}

//for type actuator
std::unique_ptr<Actuator>
SensorActuatorFactory::GetActuator(const std::string &name) {
    auto &config = Configuration::GetInstance();
    const auto type = config.GetActuatorConfig(name)->type;
    //set up to extend to more actuator types
    switch(type){
    case ActuatorType::ReactionWheel:
        return std::make_unique<ReactionWheel>(name);
    }
    return {};
}

