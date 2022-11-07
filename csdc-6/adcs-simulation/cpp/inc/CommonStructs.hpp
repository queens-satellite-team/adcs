/** 
 * @file CommonStructs.hpp
 *
 * @details strongly typed sensors and actuators
 *
 * @authors Lily de Loe
 *
 * Last Edited
 * 2022-11-03
 *
**/
#pragma once

/**
* @details enum class for sensor type, which defines all valid sensor types
*/
enum class SensorType{
    Gyroscope,
    Accelerometer
};

/**
* @details enum class for actuator type, which defines all valid actuator types
*/
enum class ActuatorType{
    ReactionWheel
};
