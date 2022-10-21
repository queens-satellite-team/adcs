/** @file Simulator.hpp
 *
 * @details This is a header file which defines the structure of the ADCS 
 *          Simulator class. All properties of the class and functionality
 *          it exposes should be detailed in this file.
 *
 * @authors Justin Paoli
 *
 * Last Edited
 * 2022-10-20
 *
**/

#pragma once

#include <vector>
#include "sim_interface.hpp"

using namespace std;

class Simulator {
    public:
        /*
        * @class Simulator
        * @param sensors [vector<Sensor>], the satelite sensors
        * @param actuators [vector<Actuator>], the satelite actuators
        * 
        * @details Constructor for the Simulator class.
        */
        Simulator(vector<Sensor> sensors, vector<Actuator> actuators);

        /*
        * @name update_simulation
        *
        * @details Updates the simulation based on the amount of time the
        * control code spent running. Used when the control code requests
        * up to date values for a sensor
        */
        void update_simulation();

        /*
        * @name set_adcs_sleep
        * @param duration [timestamp], the additional time to simulate
        * 
        * @details Updates the simulation based on the amount of time the
        * control code spent running plus some additional specified time.
        * Used when the control code is not ready for new sensor data and
        * intends to sleep until new data can be processed.
        */
        void set_adcs_sleep(timestamp duration);

    private:
        /*
        * @property simulation_time
        *
        * @details The timestamp that has so far been simulated to. Initialized 
        * to 0 at the beginning of the simulation and incremented with each
        * timestep.
        */
        timestamp simulation_time;

        /*
        * @property sensors
        *
        * @details A vector of references to the sensors present on the satellite.
        * Used to loop through the sensors after calculations and update their 
        * internal values.
        */
        vector<Sensor> sensors;

        /*
        * @property actuators
        *
        * @details A vector of references to the actuators present on the satellite.
        * Used to loop through to query each actuator for its values to inform
        * the simulation calculations.
        */
        vector<Actuator> actuators;

        /*
        * @name determine_time_passed
        * @returns timestamp
        * 
        * @details Used to determine the time the control code spent running in order
        * to account for the real-life losses due to processing speed.
        */
        timestamp determine_time_passed();

        /*
        * @name simulate
        * @param t [timestamp], the amount of time to be simulated
        * 
        * @details Used to perform the main simulation calculations. Iterates
        * over each timestep up until the specified timestamp t's worth
        * of time has been simulated.
        */
        void simulate(timestamp t);

        /*
        * @name timestep
        *
        * @details Used to perform a single timestep of simulation.
        */
        void timestep();

        /*
        * @name update_adcs_devices
        *
        * @details Iterates through all the known sensors and actuators and updates
        * their values according to the simulation to be used in the control
        * code.
        */
        void update_adcs_devices();
};