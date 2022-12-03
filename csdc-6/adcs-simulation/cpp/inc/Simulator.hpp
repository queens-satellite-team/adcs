/**
 * @file Simulator.hpp
 *
 * @details header file for class file that would configure and propagate the simulation
 *
 * @authors Lily de Loe, Justin Paoli, Aidan Sheedy
 *
 * Last Edited
 * 2022-12-01
 *
**/

#pragma once

#include <memory>
#include <vector>

#include "def_interface.hpp"
#include "CommonStructs.hpp"
#include "Messenger.hpp"

/**
 * @class Simulator
 *
 * @details class for the simulator object, which is based on the YAML config file
 *
**/
class Simulator {
public:
    /**
     * @class Simulator
     * @param configFile [string], the filename of the YAML config file
     *
     * @details constructor for the simulator class. Calls the configuration
     * singleton to instantiate all of the sensors and actuators present in the
     * simulation based on the properties of the provided YAML configuration file.
    **/
    Simulator(Messenger *messenger);

    /**
     * @name init
     *
     * @details Initializes the simulation with starting values.
    **/
    void init(sim_config initial_values, timestamp timeout, timestamp initial_timestep, bool variableTimestep, timestamp max_timestep, timestamp min_timestep);

    /**
     * @name update_simulation
     * @returns [timestamp], the simulation time at the end of calculations
     *
     * @details Updates the simulation based on the amount of time the
     * control code spent running. Used when the control code requests
     * up to date values for a sensor
    **/
    timestamp update_simulation();

    /**
     * @name determine_timestep
     *
     * @details Updates the desired timestep based on an error equation. Restricts the timestep to
     *          be within the min and max described in the YAML.
    **/
    void determine_timestep();

    /**
     * @name set_adcs_sleep
     * @param duration [timestamp], the additional time to simulate
     * @returns [timestamp], the simulation time at the end of calculations
     *
     * @details Updates the simulation based on the amount of time the
     * control code spent running plus some additional specified time.
     * Used when the control code is not ready for new sensor data and
     * intends to sleep until new data can be processed.
    **/
    timestamp set_adcs_sleep(timestamp duration);

    /**
     * @name reaction_wheel_update_desired_state
     * 
     * @details udpates the simulation with the new target state of a reaction wheel.
     * 
     * @param wheel_position location of the reaction wheel in the satellite body.
     * @param new_target the new target state of the reactino wheel.
     * 
     * @returns the time of the simulation upon return.
     * 
     * 
     * @note The simulator must figure out which reaction wheel is calling the function by it's 
     *       position - this may be changed to an ID later.
    **/
    timestamp reaction_wheel_update_desired_state(Eigen::Vector3f wheel_position, actuator_state new_target);

    /**
     * @name reaction_wheel_get_current_state
     * 
     * @details request by a reaction wheel for an update on its current state.
     * 
     * @param position location of the reaction wheel in the satellite body.
     * 
     * @returns the current state of the actuator.
     * 
     * 
     * @note The simulator must figure out which reaction wheel is calling the function by it's 
     *       position - this may be changed to an ID later.
    **/
    actuator_state reaction_wheel_get_current_state(Eigen::Vector3f position);

    /**
     * @name gyroscope_take_measurement
     * 
     * @details request by a gyroscope for an update on its current state.
     * 
     * @param measurement pointer to a measurement to fill with the current info for the sensor.
     * 
     * @returns the current time at the moment the measurement was taken.
    **/
    gyro_state gyroscope_take_measurement();

    /**
     * @name accelerometer_take_measurement
     * 
     * @details request by a accelerometer for an update on its current state.
     * 
     * @param measurement pointer to a measurement to fill with the current info for the sensor.
     * 
     * @returns the current time at the moment the measurement was taken.
    **/
    timestamp accelerometer_take_measurement(Eigen::Vector3f *measurement);

private:
    /**
     * @name simulate
     * @param t [timestamp], the amount of time to be simulated
     *
     * @details Used to perform the main simulation calculations. Iterates
     * over each timestep up until the specified timestamp t's worth
     * of time has been simulated.
    **/
    void simulate(timestamp t);

    /**
     * @name timestep
     *
     * @details Used to perform a single timestep of simulation.
    **/
    void timestep();

    /**
     * @name determine_time_passed
     * @returns timestamp
     *
     * @details Used to determine the time the control code spent running in order
     * to account for the real-life losses due to processing speed. Does not take
     * into account the processor the control code is running on.
    **/
    timestamp determine_time_passed();

private:
    /* max error allowed per timestep in position accuracy - the first term is in degrees */
    const float max_error_in_rad = 0.00005 * M_PI / 180;

    /**
     * @property simulation_time [timestamp]
     *
     * @details The timestamp that has so far been simulated to. Initialized
     * to 0 at the beginning of the simulation and incremented with each
     * timestep.
    **/
    timestamp simulation_time;

    /**
     * @property last_called [timestamp]
     *
     * @details The timestamp that the simulation was last called at. Used to determine
     * the amount of time that has passed while the control code was running.
    **/
    timestamp last_called;

    /**
     * @property timestep_length [timestamp]
     *
     * @details The resolution of the simulation. Each timestep that is calcuated
     * by the simualor will advance the simulation time by this amount.
    **/
    timestamp timestep_length;

    /**
     * @property timestep [bool]
     *
     * @details either TRUE (variable timestep) or FALSE (fixed timestep)
    **/
    bool variableTimestep;

    /**
     * @property maxStep [float]
     *
     * @details max allowed timestep from user
    **/
    timestamp max_timestep;

    /**
     * @property minStep [float]
     *
     * @details min allowed timestep from user
    **/
    timestamp min_timestamp;

    /**
     * @property system_vals [sim_config]
     *
     * @details Structure containing all the current states of each aspect of the satellite system.
    **/  
    sim_config system_vals;

    /**
     * @property messenger [Messenger*]
     *
     * @details pointer to a messenger object to rely messages and status updates to the user.
    **/  
    Messenger *messenger;

    /**
     * @property timeout [timestamp]
     * 
     * @details  maximum amount of time the simulator is allowed to run.
    **/
    timestamp timeout;
};

/**
 * @exception simulation_timeout
 *
 * @details exception used to indicate that a UI function has received bad arguments from the user.
**/
class simulation_timeout : public adcs_exception
{
    public:
        simulation_timeout(const char* msg) :  adcs_exception(msg) {}
};
