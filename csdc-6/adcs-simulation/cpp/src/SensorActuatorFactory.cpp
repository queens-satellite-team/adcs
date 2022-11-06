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
#include "Simulator.hpp"

std::shared_ptr<Sensor> SensorActuatorFactory::GetSensor(const std::string &name, Simulator* sim) {
    auto &config = Configuration::GetInstance();
    const auto type = config.GetSensorConfig(name)->type;
    std::shared_ptr<Sensor> ret = {};

    switch(type){
    case SensorType::Accelerometer:
        ret = std::make_shared<Accelerometer>(name, nullptr, sim, {0});
    case SensorType::Gyroscope:
        ret = std::make_shared<Gyroscope>(name, nullptr, sim, {0});
    }
    return ret;
}

std::shared_ptr<Actuator> SensorActuatorFactory::GetActuator(const std::string &name, Simulator* sim) {
    auto &config = Configuration::GetInstance();
    const auto type = config.GetActuatorConfig(name)->type;
    std::shared_ptr<Actuator> ret = {};

    //this is set up to extend to more actuator types
    switch(type){
    case ActuatorType::Reaction_wheel:
        ret = std::make_shared<Reaction_wheel>(name, nullptr, sim, {0}, nullptr, nullptr, {0});
    }
    return ret;
}
