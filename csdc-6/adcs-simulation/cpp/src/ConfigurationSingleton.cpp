/**
 * @file ConfigurationSingleton.cpp
 *
 * @details singleton class used to configure the user's sensor/actuator inputs
 *
 * @authors Lily de Loe
 *
 * Last Edited
 * 2022-11-08
 *
**/

#include "ConfigurationSingleton.hpp"
#include <iostream>

GyroConfig::GyroConfig(const YAML::Node &node) : SensorConfig(SensorType::Gyroscope, node) {

}

AccelerometerConfig::AccelerometerConfig(const YAML::Node &node) : SensorConfig(SensorType::Accelerometer, node) {

}

ReactionWheelConfig::ReactionWheelConfig(const YAML::Node &node) : ActuatorConfig(ActuatorType::ReactionWheel) {
    momentOfInertia = node["Moment"].as<float>();

    maxAngVel = node["MaxAngVel"].as<float>();
    maxAngAccel = node["MaxAngAccel"].as<float>();
    minAngVel = node["MinAngVel"].as<float>();
    minAngAccel = node["MinAngAccel"].as<float>();

    pollingTime = node["PollingTime"].as<float>();

    int i = 0;
    for (const auto &n : node["Position"]) {
        position(i++) = n.as<float>();
    }

    i = 0;
    for (const auto &n : node["AxisOfRotation"]) {
        axisOfRotation(i++) = n.as<float>();
    }
    velocity = node["Velocity"].as<float>();
    acceleration = node["Acceleration"].as<float>();
}

bool Configuration::Load(const std::string &configFile) {
    //load the yaml config file
    try {
        top = YAML::LoadFile(configFile);
    } catch (YAML::Exception &e) {
        std::cout << "YAML File Load failure for file: " << configFile << " : " << e.what() << std::endl;
        return false;
    }

    //load initial satellite configuration
    try {
        YAML::Node satellite = top["Satellite"];
        int i = 0;
        int j = 0;
        for (const auto &n : satellite["Moment"]) {
            j = 0;
            for (const auto &a : n) {
                satelliteMomentOfInertia(i, j++) = a.as<float>();
            }
            ++i;
        }

        i = 0;
        for (const auto &n : satellite["Position"]) {
            satellitePosition(i++) = n.as<float>();
        }

        i = 0;
        for (const auto &n : satellite["Velocity"]) {
            satelliteVelocity(i++) = n.as<float>();
        }

    } catch (YAML::Exception &e){
        std::cout << "YAML ERROR ON SATELLITE STATE: " << e.what() <<std::endl;
    }

    //load input timestep
    try {
        YAML::Node time = top["TimeStep"];
        timestepInMilliSeconds = time.as<float>();
    } catch (YAML::Exception &e){
        std::cout << "YAML ERROR ON TIMESTEP: " << e.what() <<std::endl;
    }

    //load sensors
    try {
        YAML::Node sensors = top["Sensors"];
        for (const auto &n : sensors) {
            const std::string type = n.second["type"].as<std::string>();
            if (type == "Gyroscope") {
                sensorConfigs[n.first.as<std::string>()] = std::make_shared<GyroConfig>(n.second);
            } else if (type == "Accelerometer") {
                sensorConfigs[n.first.as<std::string>()] = std::make_shared<AccelerometerConfig>(n.second);
            } else {
                std::cout << "Error unknown sensor type: " << type << std::endl;
            }
          }
    } catch (YAML::Exception &e) {
        std::cout << "YAML ERROR ON SENSORS: " << e.what() << std::endl;
    }

    //load actuators
    try {
        YAML::Node actuatorsYaml = top["Actuators"];
        for (const auto &n : actuatorsYaml) {
            const std::string type = n.second["type"].as<std::string>();
            if (type == "ReactionWheel") {
                actuatorConfigs[n.first.as<std::string>()] = std::make_shared<ReactionWheelConfig>(n.second);
            } else {
                std::cout << "Unkown actuator type: " << type << std::endl;
            }
        }
    } catch (YAML::Exception &e) {
        std::cout << "YAML ERROR ON ACTUATORS: " << e.what() << std::endl;
    }

    return true;
}
