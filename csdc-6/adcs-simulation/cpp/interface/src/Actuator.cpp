/** 
 * @file Actuator.hpp
 *
 * @details This file dimplements the Actuator class as defined in sim_interface.
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-10-11
 *
**/

#include <Eigen/Dense>

#include "sim_interface.hpp"
#include "Simulator.hpp"

Actuator::Actuator(timestamp polling_time, Simulator* sim, Eigen::Vector3f position, actuator_state max_vals, actuator_state min_vals, actuator_state initial_vals, Eigen::Vector3f axis_of_rotation) : ADCS_device(polling_time, sim)
{
    this->max_state_values = max_vals;
    this->min_state_values = min_vals;
    this->position = position;
    this->current_state = initial_vals;
    this->axis_of_rotation = axis_of_rotation;
}

actuator_state Actuator::get_target_state()
{
    return this->target_state;
}

void Actuator::set_current_state(actuator_state new_state)
{
    try
    {
        check_valid_state(new_state);
    }
    catch(const std::exception& e)
    {
        /* THROW APPROPRIATE EXCEPTION**/
    }

    this->current_state = new_state;

    return;
}

Eigen::Vector3f Actuator::get_position()
{
    return this->position;
}


void Actuator::check_valid_state(actuator_state state)
{
    if ( (this->max_state_values.acceleration < state.acceleration) ||
         (this->min_state_values.acceleration > state.acceleration) )
    {
        /* THROW EXCEPTION*/
    }

    if ( (this->max_state_values.velocity < state.velocity) ||
         (this->min_state_values.velocity > state.velocity) )
    {
        /* THROW EXCEPTION*/
    }

    if ( (this->max_state_values.position < state.position) ||
         (this->min_state_values.position > state.position) )
    {
        /* THROW EXCEPTION*/
    }

    return;
}
