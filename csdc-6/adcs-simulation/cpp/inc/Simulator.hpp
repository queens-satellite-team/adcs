/** 
 *@file Simulator.hpp
 *
 *@details header file for class file that would configure and propagate the simulation
 *
 *@authors Lily de Loe, Justin Paoli
 *
 *Last Edited
 *2022-11-03
 *
**/

#pragma once

#include "sim_interface.hpp"
#include <memory>
#include <vector>

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

class Simulator {
public:
    /** 
    * @class Simulator
    * @param configFile [string], the filename of the YAML config file
    * 
    * @details constructor for the simulator class. Calls the configuration
    * singleton to instantiate all of the sensors and actuators present in the
    * simulation based on the properties of the provided YAML configuration file.
    */
    Simulator(const std::string &configFile);

    /**
    * @name update_simulation
    * @returns [timestamp], the simulation time at the end of calculations
    *
    * @details Updates the simulation based on the amount of time the
    * control code spent running. Used when the control code requests
    * up to date values for a sensor
    */
    timestamp update_simulation();

    /**
    * @name set_adcs_sleep
    * @param duration [timestamp], the additional time to simulate
    * @returns [timestamp], the simulation time at the end of calculations
    * 
    * @details Updates the simulation based on the amount of time the
    * control code spent running plus some additional specified time.
    * Used when the control code is not ready for new sensor data and
    * intends to sleep until new data can be processed.
    */
    timestamp set_adcs_sleep(timestamp duration);

private:
    /**
    * @name create_actuator
    * @param name [string], the name of the actuator to be created
    *
    * @details creates an actuator object
    */  
    void create_actuator(const std::string &name);

    /**
    * @name creat_sensor
    * @param name [string], the name of the sensor to be created
    *
    * @details creates a sensor object
    */
    void create_sensor(const std::string &name);

    /**
    * @name simulate
    * @param t [timestamp], the amount of time to be simulated
    * 
    * @details Used to perform the main simulation calculations. Iterates
    * over each timestep up until the specified timestamp t's worth
    * of time has been simulated.
    */
    void simulate(timestamp t);

    /**
    * @name timestep
    *
    * @details Used to perform a single timestep of simulation.
    */
    void timestep();

    /**
    * @name update_adcs_devices
    *
    * @details Iterates through all the known sensors and actuators and updates
    * their values according to the simulation to be used in the control
    * code.
    */
    void update_adcs_devices();

private:
    /**
    * @property simulation_time [timestamp]
    *
    * @details The timestamp that has so far been simulated to. Initialized 
    * to 0 at the beginning of the simulation and incremented with each
    * timestep.
    */
    timestamp simulation_time;

    /**
    * @property last_called [timestamp]
    *
    * @details The timestamp that the simulation was last called at. Used to determine
    * the amount of time that has passed while the control code was running.
    */
    timestamp last_called;

    /**
    * @property timestep_length [timestamp]
    *
    * @details The resolution of the simulation. Each timestep that is calcuated
    * by the simualor will advance the simulation time by this amount.
    */
    timestamp timestep_length;

    /**
    * @property satellite [Satellite]
    *
    * @details An instance of a satellite used to start rotational positions,
    * velocities, and accelerations.
    */
    Satellite* satellite;

    /**
    * @name determine_time_passed
    * @returns timestamp
    * 
    * @details Used to determine the time the control code spent running in order
    * to account for the real-life losses due to processing speed. Does not take
    * into account the processor the control code is running on.
    */
    timestamp determine_time_passed();

    /**
    * @details unordered map of sensors that relates strings to names
    */
    std::unordered_map<std::string, std::unique_ptr<Sensor>> sensors;

    /**
    * @details unordered map of actuators that relates strings to names
    */
    std::unordered_map<std::string, std::unique_ptr<Actuator>> actuators;
};
