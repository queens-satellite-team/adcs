/**
 * @file PointingModeController.hpp
 *
 * @details Header file for the pointing mode controller
 *
 * @authors Justin Paoli, Aidan Sheedy
 *
 * Last Edited
 * 2022-11-07
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
        std::unordered_map<std::string, std::shared_ptr<Actuator>> actuators
    );

    /**
    * @name begin
    * @param desired_attitue [vector<float>], the desired angle to point at
    *
    * @details Starts the command loop. Should call the update functions once per cycle to get
    * updated sensor measurements and sent commands to actuators accordingly
   **/
    void begin(std::vector<float> desired_attitude);

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
    * @name take_updated_measurements
    *
    * @details Iterates through all of the sensors, taking updated measurements from
    * any that are ready.
    *
    * TODO: should possibly not be void? Depends on if we want to store the sensor
    * measurements as class properties or have this function return them.
   **/
    void take_updated_measurements();
};
