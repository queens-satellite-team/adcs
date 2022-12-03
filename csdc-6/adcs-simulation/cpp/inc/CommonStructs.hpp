/**
 * @file CommonStructs.hpp
 *
 * @details strongly typed sensors and actuators
 *
 * @authors Lily de Loe, Aidan Sheedy
 *
 * Last Edited
 * 2022-11-08
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
* @property inertia_b [Eigen::Matrix3f], the inertia tensor of the satellite body
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

/**
 * @struct  sim_reaction_wheel
 * 
 * @details structure defining the status of a reaction wheel in the simulator.
 * 
 * @param omega     the angular velocity of the reaciton wheel (body frame)
 * @param alpha     the angular acceleration of the reaction wheel (body frame)
 * @param inertia   the inertia matrix of the reaction wheel.
 * @param position  the position of the satellite of the reaction wheel
 * 
**/
typedef struct
{
    float omega;
    float alpha;
    float inertia;
    Eigen::Vector3f axis_of_rotation;
    Eigen::Vector3f position;
} sim_reaction_wheel;

/**
 * @struct  sim_accelerometer
 * 
 * @details structure defining the status of an accelerometer in the simulator.
 * 
 * @param measurement latest "measured" value of the accelerometer.
 * @param position    the position of the satellite of the accelerometer.
 * 
**/
typedef struct
{
    Eigen::Vector3f measurement;
    Eigen::Vector3f position;
} sim_accelerometer;

/**
 * @struct  sim_gyroscope
 * 
 * @details structure defining the status of an gyroscope in the simulator.
 * 
 * @param measurement latest "measured" value of the gyroscope.
 * @param position    the position of the satellite of the gyroscope.
 * 
**/
typedef struct
{
    Eigen::Vector3f theta;
    Eigen::Vector3f omega;
    Eigen::Vector3f alpha;
    Eigen::Vector3f position;
} sim_gyroscope;

/**
 * @struct  sim_config
 * 
 * @details structure defining the status of the entire satellite system in the simulation.
 * 
 * @param satellite         info of the overall satellite
 * @param accelerometer     accelerometer info in the satellite system
 * @param gyroscope         gyroscope info in the satellite system
 * @param reaction_wheels   vector of all reaction wheels in the satellite system
 * 
**/
typedef struct
{
    Satellite                        satellite;
    sim_accelerometer                accelerometer;
    sim_gyroscope                    gyroscope;
    std::vector<sim_reaction_wheel>  reaction_wheels;
} sim_config;

/**
 * @struct  text_colour
 *
 * @details structure containing the ANSI escape codes for the desired text colour.
 *
 * @property reset  resets the colour to the default
 * @property red    sets the colour to red
 * @property green  sets the colour to green
 * @property yellow sets the colour to yellow
**/
static const struct
{
    std::string reset   = "\033[0m";
    std::string red     = "\033[31m";
    std::string green   = "\033[32m";
    std::string yellow  = "\033[33m";
    std::string magenta = "\033[35m";
    std::string cyan    = "\033[36m";
} text_colour;
