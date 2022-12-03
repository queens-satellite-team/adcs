/** 
 * @file    Reaction_wheel.cpp
 *
 * @details This file implements the reaction wheel definition as defined in sim_interface.hpp.
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-11-18
 *
**/

#include <Eigen/Dense>
#include <iostream>

#include "sim_interface.hpp"
#include "Simulator.hpp"

Reaction_wheel::Reaction_wheel(timestamp polling_time, Simulator* sim, Eigen::Vector3f position, actuator_state max_vals, actuator_state min_vals, actuator_state initial_vals, Eigen::Vector3f axis_of_rotation, float inertia_matrix) : Actuator(polling_time, sim, {position}, max_vals, min_vals, initial_vals, axis_of_rotation)
{

    if(inertia_matrix == 0)
    {
        throw invalid_adcs_param("Reaction wheel inertia is 0. (YEET was not succesful)");
    }

    this->inertia_matrix = inertia_matrix;

    return;
}

float Reaction_wheel::get_inertia_matrix()
{
    return this->inertia_matrix;
}

void Reaction_wheel::set_target_state(actuator_state new_target)
{
    check_valid_state(new_target);

    if (this->time_until_ready() > 0)
    {
        throw device_not_ready("Reaction wheel not ready.");
    }
    this->target_state = new_target;
    timestamp cur_time = this->sim->reaction_wheel_update_desired_state(this->position, this->target_state);
    this->update_poll_time(cur_time);

    return;
}

actuator_state Reaction_wheel::get_current_state()
{
    this->sim->reaction_wheel_get_current_state(this->position);
    return this->current_state;
}

