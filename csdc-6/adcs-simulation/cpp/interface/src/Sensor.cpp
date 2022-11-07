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

Sensor::Sensor(timestamp polling_time, vector<Eigen::Vector3f> positions, uint32_t num_sensors, uint32_t num_axes) : ADCS_device(polling_time)
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


measurement Sensor::take_measurement(timestamp curr_time)
{
    if (this->time_until_ready(curr_time) > 0)
    {
        /* THROW APPROPRIATE EXCEPTION */
    }

    return this->current_vector_value;
}

void Sensor::set_current_vals(vector<Eigen::VectorXf> physical_vals, timestamp time)
{
    if ( (3 != num_axes)                  ||
         (1 != physical_vals.size())      ||
         (3 != physical_vals.at(0).size()))
    {
        /* SETUP DOES NOT MEET ASSUMPTIONS - THROW EXCEPTION*/
    }

    /* Default behaviour is to assume that the physical values require no modification. */
    int vals_length = physical_vals.at(0).size();
    vector<float> values_vector{physical_vals.at(0).data(), physical_vals.at(0).data() + vals_length};

    current_vector_value.time_taken = time;
    current_vector_value.vec = values_vector;

    return;
}

vector<Eigen::Vector3f> Sensor::get_positions()
{
    return positions;
}
