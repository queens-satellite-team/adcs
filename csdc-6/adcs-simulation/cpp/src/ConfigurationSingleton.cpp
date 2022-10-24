/** @file ConfigurationSingleton.cpp
 *
 *@details singleton class used to configure the user's sensor/actuator inputs
 *
 *@authors Lily de Loe
 *
 *Last Edited
 *2022-10-24
 *
**/
#include "ConfigurationSingleton.hpp"
#include <iostream>

//sensor, actuator config constructors
//node is commented out so an exception isn't thrown
GyroConfig::GyroConfig(const YAML::Node &/*node*/)
    : SensorConfig(SensorType::Gyroscope) { 
      /* code goes here, uncomment node once there's code */ }

AccelerometerConfig::Config(const YAML::Node & /*node*/)
    : SensorConfig(SensorType::Accelerometer) { 
      /* code goes here, uncomment node once there's code */ }

ReactionWheelConfig::ReactionWheelConfig(const YAML::Node & /*node*/)
    : ActuatorConfig(ActuatorType::Actuator) { 
      /* code goes here, uncomment node once there's code */ }

//sensor configuration
bool Configuration::Load(const std::string &configFile) {
    //load the yaml config file
    try {
        top = YAML::LoadFile(configFile);
    } catch (YAML::Exception &e) {
        std::cout << "YAML File Load failure for file: " << configFile << " : "
        << e.what() << std::endl;
        return false;
    }
  
    //load sensors
    try {
        YAML::Node sensors = top["Sensors"];
        for (const auto &n : sensors) {
            const std::string type = n.second["type"].as<std::string>();
            if (type == "Gyroscope") {
                sensorConfigs[n.first.as<std::string>()] = 
                std::make_shared<GyroConfig>(n.second);
            } else if (type == "Accelerometer") {
                sensorConfigs[n.first.as<std::string>()] = 
                std::make_shared<AccelerometerConfig>(n.second);
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
                actuatorConfigs[n.first.as<std::string>()] = 
                std::make_shared<ReactionWheelConfig>(n.second);
            } else {
                std::cout << "Unkown actuator type: " << type << std::endl;
            }
        }
    } catch (YAML::Exception &e) {
        std::cout << "YAML ERROR ON ACTUATORS: " << e.what() << std::endl;
    }

    //these are required for use of timestep and tick
    /*
    try {
        timeStep = top["TimeStep"].as<double>();
    } catch (YAML::Exception &e) {
        std::cout << "TimeStep not found" << std::endl;
        return false;
    }

    try {
        tickPeriod = top["TickPeriod"].as<double>();
    } catch (YAML::Exception &e) {
        std::cout << "TickPeriod not found" << std::endl;
        return false;
    }
    */
    return true;
}

