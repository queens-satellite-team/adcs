/**
 * @file PointingModeController.hpp
 *
 * @details Header file for the pointing mode controller
 *
 * @authors Justin Paoli, Aidan Sheedy
 *
 * Last Edited
 * 2022-11-13
**/

#pragma once

#include <unordered_map>
#include "interface.hpp"

class PointingModeController {
public:
    /**
    * @class PointingModeController
    * @param sensors [unordered_map<string, shared_ptr<Sensor>], pointers to the satellite sensors
    * @param actuators [unordered_map<string, shared_ptr<Actuator>], pointers to the satellite actuators
    *
    * @details Constructor for the pointing mode controller class. Initializes the internal references
    * to the satellite sensors and actuators which will be used to request information from the sensors
    * and send commands to the actuators.
   **/
    PointingModeController(
        std::unordered_map<std::string, std::shared_ptr<Sensor>> sensors,
        std::unordered_map<std::string, std::shared_ptr<Actuator>> actuators,
        ADCS_timer *timer
    );

    /**
    * @name begin
    * @param desired_attitue [vector<float>], the desired angle to point at
    * @param ramp_time [timestamp], the time over which to ramp to the new desired attitude
    *
    * @details Starts the command loop. Should call the update functions once per cycle to get
    * updated sensor measurements and sent commands to actuators accordingly
   **/
    void begin(Eigen::Vector3f desired_attitude, timestamp ramp_time);

private:
    /**
    * @property sensors [unordered_map<string, shared_ptr<Sensor>>]
    *
    * @details An unordered map containing references to the satellite sensors.
   **/
    std::unordered_map<std::string, std::shared_ptr<Sensor>> sensors;

    /**
    * @property actuators [unordered_map<string, shared_ptr<Actuator>>]
    *
    * @details An unordered map containing references to the satellite actuators.
   **/
    std::unordered_map<std::string, std::shared_ptr<Actuator>> actuators;

    /**
    * @property gyro [Gyroscope *]
    *
    * @details A pointer to the gyroscope as this is currently the only sensor
    * for which measurements are actually used.
   **/
    Gyroscope *gyro;

    /**
    * @property prev_error [Eigen::Vector3f]
    *
    * @details The error term, stored to persist between controller timesteps
   **/
    Eigen::Vector3f prev_error;

    /**
    * @property prev_derivative [Eigen::Vector3f]
    *
    * @details The derivative term, stored to persist between controller timesteps
   **/
    Eigen::Vector3f prev_derivative;

    /**
    * @property prev_integral [Eigen::Vector3f]
    *
    * @details The integral term, stored to persist between controller timesteps
   **/
    Eigen::Vector3f prev_integral;

    /**
    * @property A_gen_inv [Eigen::Matrix3Xf]
    *
    * @details The matrix A stores the rotational axes of each reaction wheel, one
    * wheel per column. A_gen_inv is the generalised inverse of the matrix A, which
    * is used to split the desired torques, as calculated by the controller, into
    * individual scalar torques that should be applied by each reaction wheel.
   **/
    Eigen::Matrix3Xf A_gen_inv;

    ADCS_timer *timer;

    /**
    * @name take_updated_measurements
    * @returns [measurement]
    *
    * @details For now just checks the gyroscope to get an updated current attitude
    * of the satellite.
   **/
    measurement take_updated_measurements();

    /**
    * @name update
    *
    * @details One cycle of the PID controller iteration. Recalculates the desired torque
    * based on the current attitude of the satellite and updates the accelerations of the 
    * reaction wheels.
   **/
    void update(Eigen::Vector3f current_attitude, Eigen::Vector3f desired_attitude, timestamp delta_t);
};
