/**
 * @file ConfigurationSingleton.hpp
 *
 * @details hpp file for singleton class used to configure the user's sensor/actuator inputs
 *
 * @authors Lily de Loe
 *
 * Last Edited
 * 2022-11-03
 *
**/

#pragma once

#include <string>
#include <unordered_map>

#include "CommonStructs.hpp"
#include <yaml-cpp/yaml.h>
#include <Eigen/Dense>

/**
* @name SensorConfig
* @property type [SensorType], the sensor type
*
* @details struct outlining the sensor configuration according to the input sensor
* config and type
*/
struct SensorConfig {
    SensorConfig(SensorType t, const YAML::Node &node) : type(t){
        pollingTime = node["PollingTime"].as<double>();
        int j = 0;
        for (const auto &n : node["Position"]) {
            position(j++) = n.as<double>();
        }
    };
    virtual ~SensorConfig() = default;
    float pollingTime;
    SensorType type;
    Eigen::Vector3f position;
};

/**
* @name ActuatorConfig
* @property type [ActuatorType], the actuator type
*
* @details struct outlining the actuator configuration according to the input actuator
* config and type
*/
struct ActuatorConfig {
    ActuatorConfig(ActuatorType t) : type(t){};
    virtual ~ActuatorConfig() = default;
    ActuatorType type;
};

/**
* @name GryoConfig
* @property pollingTime [float] the polling time of the gyroscope
* @property position [Eigen::Matrix3f] the position of the sensor
*
* @details struct outling the configuration of a gryoscope according to the input YAML
* parameters
*/
struct GyroConfig : public SensorConfig {
    GyroConfig(const YAML::Node &node);

};

/**
* @name AccelerometerConfig
* @property pollingTime [float] the polling time of the accelerometer
* @property position [Eigen::Matrix3f] the position of the sensor
*
* @details struct outling the configuration of an accelerometer according to the input YAML
* parameters
*/
struct AccelerometerConfig : public SensorConfig {
    AccelerometerConfig(const YAML::Node &node);

};

/**
 * @name ReactionWheelConfig
 * @property momentOfInertia [Eigen::Matrix3f], matrix representing the moment of inertia
 * @property maxAngVel [float], maximium angular velocity
 * @property maxAngAccel [float], maximimum angular acceleration
 * @property minAngVel [float], minimium angular velocity
 * @property minAngAccel [float], minimimum angular acceleration
 * @property pollingTime [float], polling time of the reaction wheel
 * @property position [Eigen::Vector3f], unit position vector associated with the reaction wheel
 *
 * @details struct outling the configuration of a reaction wheel according to the input YAML
* parameters
*/
struct ReactionWheelConfig : public ActuatorConfig {
    ReactionWheelConfig(const YAML::Node &node);

    Eigen::Matrix3f momentOfInertia;
    float maxAngVel;
    float maxAngAccel;
    float minAngVel;
    float minAngAccel;
    float pollingTime;
    Eigen::Vector3f position;

};

/**
 * @class Configuration
 *
 * @details class used in configuring the satellite based on its input YAML file
 *
**/
class Configuration {
public:
    /**
    * @name Load
    * @param fileName [string], the input YAML file's name
    * @details loads the input YAML file
   **/
    bool Load(const std::string &fileName);

    /**
    * @details copy constructor is explicitly deleted to prevent C++ from auto-generating one
   **/
    Configuration(const Configuration &) = delete;

    /**
    * @details equals operator is explicitly deleted to prevent C++ from auto-generating one
   **/
    void operator=(Configuration &) = delete;

    /**
    * @name GetInstance
    * @return instance of the satellite configuration
    *
    * @details getter for an instance of the satellite's configuration
   **/
    inline static Configuration &GetInstance() {
        static Configuration instance;
        return instance;
    };

    /**
    * @name GetSensorConfig
    * @param name [string], the name of the sensor
    * @return the sensor config
    *
    * @details getter for shared pointer to the sensor config
   **/
    inline const std::shared_ptr<SensorConfig> &GetSensorConfig(const std::string &name) {
        return sensorConfigs[name];
    };

    /**
    * @name GetActuatorConfig
    * @param name [string], the name of the actuator
    * @return the actuator config
    *
    * @details getter for shared pointer to the actuator config
   **/
    inline const std::shared_ptr<ActuatorConfig> &GetActuatorConfig(const std::string &name) {
        return actuatorConfigs[name];
    };

    /**
    * @name GetSensorConfigs
    * @return the sensor configs
    *
    * @details getter for the map of sensor configs
   **/
    inline const std::unordered_map<std::string, std::shared_ptr<SensorConfig>> &GetSensorConfigs() {
        return sensorConfigs;
    };

    /**
    * @name GetActuatorConfigs
    * @return the actuator configs
    *
    * @details getter for the map of actuator configs
   **/
    inline const std::unordered_map<std::string, std::shared_ptr<ActuatorConfig>> &GetActuatorConfigs() {
        return actuatorConfigs;
    };

    /**
    * @name GetSatelliteMoment
    * @return the Inertia matrix for the satellite
    *
    * @details getter for shared pointer to the satellite's moment of inertia
   **/
    inline const Eigen::Matrix3f &GetSatelliteMoment() {
        return satelliteMomentOfInertia;
    };

    /**
    * @name GetSatellitePosition
    * @return the 3-dimensional vector representing satellite position
    *
    * @details getter for shared pointer to the satellite's position
   **/
    inline const Eigen::Vector3f &GetSatellitePosition() {
        return satellitePosition;
    };

    /**
    * @name GetSatelliteVelocity
    * @return the 3-dimensional vector representing satellite velocity
    *
    * @details getter for shared pointer to the satellite's velocity
   **/
    inline const Eigen::Vector3f &GetSatelliteVelocity() {
        return satelliteVelocity;
    };


private:
    /**
    * @name Configuration
    * @details hidden default constructor for the satellite configuration
   **/
    Configuration(){};

private:
    /**
    * @details YAML node for the top-most heading in the YAML input file
   **/
    YAML::Node top;

    /**
    * @details unordered map of sensor configs that relates strings to names
   **/
    std::unordered_map<std::string, std::shared_ptr<SensorConfig>> sensorConfigs;

    /**
    * @details unordered map of actuator configs that relates strings to names
   **/
    std::unordered_map<std::string, std::shared_ptr<ActuatorConfig>> actuatorConfigs;

    /**
    * @details 3-dimensional matrix storing the satellite's moment of inertia
   **/
    Eigen::Matrix3f satelliteMomentOfInertia;

    /**
    * @details 3-dimensional vector storing the satellite's position
   **/
    Eigen::Vector3f satellitePosition;

    /**
    * @details 3-dimensional matrix storing the satellite's velocity
   **/
    Eigen::Vector3f satelliteVelocity;
};
