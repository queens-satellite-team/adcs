/** 
 * @file ConfigurationSingleton.cpp
 *
 * @details singleton class used to configure the user's sensor/actuator inputs
 *
 * @authors Lily de Loe
 *
 * Last Edited
 * 2022-11-03
 *
**/

#include "ConfigurationSingleton.hpp"
#include <iostream>

GyroConfig::GyroConfig(const YAML::Node &node) : SensorConfig(SensorType::Gyroscope) {
    pollingTime = node["PollingTime"].as<double>();
    int i = 0;
    int j = 0;
    for (const auto &n : node["Position"]) {
        j = 0;
        for (const auto &a : n) {
            position(i, j++) = a.as<double>();
        }
        ++i;
    }
}

AccelerometerConfig::Config(const YAML::Node &node) : SensorConfig(SensorType::Accelerometer) {
    pollingTime = node["PollingTime"].as<double>();
    int i = 0;
    int j = 0;
    for (const auto &n : node["Position"]) {
        j = 0;
        for (const auto &a : n) {
            position(i, j++) = a.as<double>();
        }
        ++i;
    }   
}

ReactionWheelConfig::ReactionWheelConfig(const YAML::Node &node) : ActuatorConfig(ActuatorType::Actuator) {
    int i = 0;
    int j = 0;
    for (const auto &n : node["Moment"]) {
        j = 0;
        for (const auto &a : n) {
            momentOfInertia(i, j++) = a.as<double>();
        }
        ++i;
    }

    maxAngVel = node["MaxAngVel"].as<double>();
    maxAngAccel = node["MaxAngAccel"].as<double>();
    minAngVel = node["MinAngVel"].as<double>();
    minAngAccel = node["MinAngAccel"].as<double>();
    
    pollingTime = node["PollingTime"].as<double>();
    
    i = 0;
    for (const auto &n : node["Position"]) {
        position(i++) = n.as<double>();
    }
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
                satelliteMomentOfInertia(i, j++) = a.as<double>();
            }
            ++i;
        }

        i = 0;
        for (const auto &n : satellite["Position"]) {
            satellitePosition(i++) = n.as<double>();
        }

        i = 0;
        for (const auto &n : satellite["Velocity"]) {
            satelliteVelocity(i++) = n.as<double>();
        }        

    } catch (YAML::Exception &e){
        std::cout << "YAML ERROR ON SATELLITE STATE: " << e.what() <<std::endl;
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
