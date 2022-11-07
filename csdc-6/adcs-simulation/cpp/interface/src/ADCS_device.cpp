/** @file 	ADCS_device.hpp
 *
 * @details This file defines the interface between the control code and the simulation. It expands
 *          interface.hpp by adding simulation specific functions as required to the interface.
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

ADCS_device::ADCS_device(timestamp polling_time, Simulator* sim) : min_polling_increment(polling_time)
{
	if (NULL == sim)
	{
		/* THROW APPROPRIATE EXCEPTION */
	}

	this->sim = sim;
}

timestamp ADCS_device::time_until_ready()
{
	timestamp ret = 0;
	timestamp current_time = sim->update_simulation();
	timestamp difference = current_time - this->last_polled;

	if (difference < this->min_polling_increment)
	{
		ret = this->min_polling_increment - difference;
	}

	return ret;
}

void ADCS_device::update_poll_time(timestamp new_time)
{
	this->last_polled = new_time;
}
