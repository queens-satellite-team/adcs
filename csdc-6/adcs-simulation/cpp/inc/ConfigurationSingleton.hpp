/** @file ConfigurationSingleton.hpp
 *
 *@details hpp file for singleton class used to configure the user's sensor/actuator inputs
 *
 *@authors Lily de Loe
 *
 *Last Edited
 *2022-10-24
 *
**/

#pragma once

#include <string>
#include <unordered_map>

#include "CommonStructs.hpp"
#include <yaml-cpp/yaml.h>

struct SensorConfig {
    SensorConfig(SensorType t) : type(t){};
    SensorType type;
};
struct ActuatorConfig {
    ActuatorConfig(ActuatorType t) : type(t){};
    ActuatorType type;
};

struct GyroConfig {
    GyroConfig(const YAML::Node &node);
};

struct AccelerometerConfig {
    AccelerometerConfig(const YAML::Node &node);
};

struct ReactionWheelConfig {
    ReactionWheelConfig(const YAML::Node &node);
};

class Configuration {
public:
    bool Load(const std::string &fileName);

    Configuration(const Configuration &) = delete;
    void operator=(Configuration &) = delete;
    inline static Configuration &GetInstance() {
        static Configuration instance;
        return instance;
    };

    //get an individual sensor/actuator
    inline const std::shared_ptr<SensorConfig> &GetSensorConfig(const std::string &name) {
        return sensorConfigs[name];
    };
    inline const std::shared_ptr<ActuatorConfig> &GetActuatorConfig(const std::string &name) {
        return actuatorConfigs[name];
    };

    //get all the sensors/actuators
    inline const std::unordered_map<std::string, std::shared_ptr<SensorConfig>> &GetSensorConfigs() {
        return sensorConfigs;
    };
    inline const std::unordered_map<std::string, std::shared_ptr<ActuatorConfig>> &GetActuatorConfigs() {
        return actuatorConfigs;
    };  
  
    //use to print stats
    inline bool IsPrintStats(){return printStats;};


private:
    Configuration(){};

private:
    //YAML uses nodes
    YAML::Node top;

    std::unordered_map<std::string, std::shared_ptr<SensorConfig>> sensorConfigs;
    std::unordered_map<std::string, std::shared_ptr<ActuatorConfig>> actuatorConfigs;

    //use for printStats()
    bool printStats;
};
