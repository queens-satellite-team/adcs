/**
 * @file ConfigurationSingleton.cpp
 *
 * @details singleton class used to configure the user's sensor/actuator inputs
 *
 * @authors Lily de Loe
 *
 * Last Edited
 * 2022-12-01
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

    pollingTime = node["PollingTime"].as<int>();

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


    //load condition for variable timestep
    try {
        YAML::Node variableTimestep = top["VariableTimestep"];
        useVariableTimestep = variableTimestep.as<bool>();
    } catch (YAML::Exception &e){
        std::cout << "YAML ERROR ON VARIABLE TIMESTEP: " << e.what() <<std::endl;
    }

    if (useVariableTimestep == true) {
    //load max and min timestep
        try {
            YAML::Node max = top["TimeStepMax"];
            timeStepMax = max.as<int>();
            YAML::Node min = top["TimeStepMin"];
            timeStepMin = min.as<int>();
        } catch (YAML::Exception &e){
            std::cout << "YAML ERROR ON TIMESTEP BOUNDS: " << e.what() <<std::endl;
        }
    }

    if (useVariableTimestep == false) {
        //load input timestep
        try {
            YAML::Node time = top["TimeStep"];
            timestepInMilliSeconds = time.as<float>();
        } catch (YAML::Exception &e){
            std::cout << "YAML ERROR ON TIMESTEP: " << e.what() <<std::endl;
        }
    }
    

    //load timeout
    try {
        YAML::Node timeout = top["Timeout"];
        timeoutInMilliseconds = timeout.as<int>();
    } catch (YAML::Exception &e){
        std::cout << "YAML ERROR ON TIMEOUT: " << e.what() <<std::endl;
    }

    //load timeout
    try {
        YAML::Node timeout = top["Timeout"];
        timeoutInMilliseconds = timeout.as<int>();
    } catch (YAML::Exception &e){
        std::cout << "YAML ERROR ON TIMEOUT: " << e.what() <<std::endl;
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

bool Configuration::load_exit_file(const std::string &fileName)
{
    /* load the yaml config file */
    try 
    {
        top = YAML::LoadFile(fileName);
    } 
    catch (YAML::Exception &e)
    {
        std::cout << "YAML File Load failure for file: " << fileName << " : " << e.what() << std::endl;
        return false;
    }

    /* get the final satellite configuration */
    try
    {
        YAML::Node satellite = top["Satellite"];
 
        /* loop through each axis of desired position */
        try 
        {
            int i = 0;
            for (const YAML::Node element : satellite["DesiredPosition"])
            {
                desiredSatellitePosition(i++) = element.as<float>();
            }
        }
        catch (YAML::Exception &e)
        {
            std::cout << "YAML ERROR ON DesiredPosition: " << e.what() <<std::endl;
        }

        /* get the allowed jitter */
        try 
        {
            YAML::Node jitter = satellite["AllowedJitter"];
            this->allowed_jitter = jitter.as<float>();
        }
        catch (YAML::Exception &e)
        {
            std::cout << "YAML ERROR ON AllowedJitter: " << e.what() <<std::endl;
        }

        /* get the allowed required accuracy */
        try 
        {
            YAML::Node accuracy = satellite["RequiredAccuracy"];
            this->required_accuracy = accuracy.as<float>();
        }
        catch (YAML::Exception &e)
        {
            std::cout << "YAML ERROR ON RequiredAccuracy: " << e.what() <<std::endl;
        }
    }
    catch (YAML::Exception &e)
    {
        std::cout << "YAML ERROR ON exit Satellite: " << e.what() <<std::endl;
    }

    /* get the amount of time the controller must hold the position for */
    try
    {
        YAML::Node hold_time = top["HoldTime"];
        this->required_hold_time = hold_time.as<int>();
    }
    catch (YAML::Exception &e)
    {
        std::cout << "YAML ERROR ON HoldTime: " << e.what() <<std::endl;
    }

    return true;
}
