/** @file Actuator.hpp
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

#include <sim_interface.hpp>
#include <Simulator.hpp>

Actuator::Actuator(timestamp polling_time, Simulator* sim, vector<float> position, actuator_state max_vals, actuator_state min_vals) : ADCS_device(polling_time, sim)
{
    this->max_state_values = max_vals;
    this->min_state_values = min_vals;
    this->postition = position;
}

actuator_state Actuator::get_current_state() 
{
    return this->current_state;
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
        /* THROW APPROPRIATE EXCEPTION */
    }

    this->current_state = new_state;

    return;
}

void Actuator::set_target_state(actuator_state new_target)
{
    try
    {
        check_valid_state(new_target);
    }
    catch(const std::exception& e)
    {
        /* THROW APPROPRIATE EXCEPTION */
    }

    if (this->time_until_ready() > 0)
    {
        /* THROW APPROPRIATE EXCEPTION */
    }

    this->target_state = new_target;
    timestamp cur_time = this->sim->update_simulation();
    this->update_poll_time(cur_time);

    return;
}

vector<float> Actuator::get_postition()
{
    return this->postition;
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
