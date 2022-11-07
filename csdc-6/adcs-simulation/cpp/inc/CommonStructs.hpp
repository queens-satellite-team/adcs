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

#include <Eigen/Dense>
#include <vector>

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

/**
* @name Satellite
* @property theta_b [Eigen::Vector3f], the angular position of the satellite body
* @property omega_b [Eigen::Vector3f], the angular velocity of the satellite body
* @property alpha_b [Eigen::Vector3f], the angular acceleration of the satellite body
* @properft inertia_b [Eigen::Matrix3f], the inertia tensor of the satellite body
*
* @details A model of the values controlling the rotational kinematics of the
* satellite. Used internally by the simulator to keep track of the satellite
* proper so the sensor values can be calculated. All values are stored as Cartesian
* coordinates in an arbitrary inertial frame of reference.
*/
typedef struct {
    Eigen::Vector3f theta_b;
    Eigen::Vector3f omega_b;
    Eigen::Vector3f alpha_b;
    Eigen::Matrix3f inertia_b;
} Satellite;

typedef struct
{
    Eigen::Vector3f omega;
    Eigen::Vector3f alpha;
    Eigen::Matrix3f inertia;
    Eigen::Vector3f position;
} sim_reaction_wheel;

typedef struct
{
    Eigen::Vector3f measurement;
    Eigen::Vector3f position;
} sim_accelerometer;

typedef struct
{
    Eigen::Vector3f measurement;
    Eigen::Vector3f position;
} sim_gyroscope;

typedef struct
{
    Satellite                   satellite;
    sim_accelerometer           accelerometer;
    sim_gyroscope               gyroscope;
    std::vector<sim_reaction_wheel>  reaction_wheels;
} sim_config;
