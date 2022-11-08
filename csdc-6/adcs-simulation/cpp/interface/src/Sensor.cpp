/** @file   Sensor.hpp
 *
 * @details This file implements the Sensor class as defined in sim_interface.
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-10-11
 *
**/

#include "sim_interface.hpp"
#include "Simulator.hpp"

Sensor::Sensor(timestamp polling_time, Simulator* sim, std::vector<Eigen::Vector3f> positions, uint32_t num_sensors, uint32_t num_axes) : ADCS_device(polling_time, sim)
{
    if (num_sensors != positions.size())
    {
        /* THROW EXCEPTION */
    }

    for (int i = 0; i < num_sensors; i++)
    {
        if (num_axes != positions.at(i).size())
        {
            /* THROW EXCEPTION */
        }
    }

    this->positions     = positions;
    this->num_sensors   = num_sensors;
    this->num_axes      = num_axes;
}

void Sensor::set_current_vals(std::vector<Eigen::VectorXf> physical_vals, timestamp time)
{
    if ( (3 != num_axes)                  ||
         (1 != physical_vals.size())      ||
         (3 != physical_vals.at(0).size()))
    {
        /* SETUP DOES NOT MEET ASSUMPTIONS - THROW EXCEPTION*/
    }

    /* Default behaviour is to assume that the physical values require no modification. */
    current_vector_value.time_taken = time;
    current_vector_value.vec = physical_vals.at(0);

    return;
}

std::vector<Eigen::Vector3f> Sensor::get_positions()
{
    return positions;
}
