/**
 * @file    SensorActuatorFactory.cpp
 *
 * @details class file to create the sensor and actuator objects
 *
 * @authors Lily de Loe
 *
 * Last Edited
 * 2022-11-08
 *
**/

#include "sim_interface.hpp"
#include "SensorActuatorFactory.hpp"
#include "ConfigurationSingleton.hpp"
#include "Simulator.hpp"
#include <iostream>

std::shared_ptr<Sensor> SensorActuatorFactory::GetSensor(const std::string &name, Simulator* sim) {
    auto &config = Configuration::GetInstance();
    const auto & sensor = config.GetSensorConfig(name);

    std::shared_ptr<Sensor> ret;
    if(!sensor)
    {
        return ret;
    }

    switch(sensor->type){
    case SensorType::Accelerometer:
        ret = std::make_shared<Accelerometer>(timestamp(sensor->pollingTime, 0), sim, sensor->position);
        break;
    case SensorType::Gyroscope:
        ret = std::make_shared<Gyroscope>(timestamp(sensor->pollingTime, 0), sim, sensor->position);
        break;
    }
    return ret;
}

std::shared_ptr<Actuator> SensorActuatorFactory::GetActuator(const std::string &name, Simulator* sim) {
    auto &config = Configuration::GetInstance();
    const auto& actu = config.GetActuatorConfig(name);
    std::shared_ptr<Actuator> ret ;

    if(!actu)
    {
        return ret;
    }

    //this is set up to extend to more actuator types
    switch(actu->type){
    case ActuatorType::ReactionWheel:
        {
            const ReactionWheelConfig* reac = dynamic_cast<const ReactionWheelConfig*>(actu.get());
            actuator_state min;
            min.acceleration = reac->minAngAccel;
            min.velocity = reac->minAngVel;
            min.position = -std::numeric_limits<float>::max();
            min.time = timestamp(0.0f);
            actuator_state max;
            max.acceleration = reac->maxAngAccel;
            max.velocity = reac->maxAngVel;
            max.position = std::numeric_limits<float>::max();
            max.time = timestamp(std::numeric_limits<float>::max());
            actuator_state initial_vals;
            initial_vals.acceleration = reac->acceleration;
            initial_vals.position = 0; //this is unused
            initial_vals.time = 0; //this is unused
            initial_vals.velocity = reac->velocity;
            ret = std::make_shared<Reaction_wheel>(timestamp(reac->pollingTime, 0), sim, reac->position, max, min, initial_vals, reac->axisOfRotation, reac->momentOfInertia);
            break;
        }
    }
    return ret;
}
