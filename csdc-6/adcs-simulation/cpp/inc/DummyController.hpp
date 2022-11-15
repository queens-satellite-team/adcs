/**
 * @file    DummyController.hpp
 *
 * @details Header file for the dummy controller
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-11-14
**/

#pragma once

#include <unordered_map>

#include "interface.hpp"

/**
 * Runs the simulation on an infinite loop -> or at least until the default maximum timeout of 1h.
 * 
 * @param timer [ADCS_timer*] timer used to update the simulation.
**/
class DummyController {
public:
    /**
     * @details Constructor for the DummyController class. Saves the timer pointer.
     *
     * @param timer timer used to update the simulation.
     *
    **/
    DummyController(ADCS_timer *timer);

    /**
     * @details delete default constructor for the DummyController class to avoid seg faults
    **/
    DummyController() = delete;

    /**
     * @name begin
     *
     * @details Starts the command loop. Should calls the timer once per loop to update time sim.
     *
     * @param desired_attitue [vector<float>], the desired angle to point at
     *
    **/
    void begin();

private:
    /* Timer used to update the simulation. */
    ADCS_timer *timer;

    /* Hard timeout limit is set to 1 hour of simulation. */
    timestamp timeout = timestamp(0x36EE80, 0);
};
