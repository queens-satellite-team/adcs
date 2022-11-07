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

#include <Eigen/Dense>

#include "sim_interface.hpp"

measurement Accelerometer::take_measurement()
{
    if (this->time_until_ready() > 0)
    {
        /* THROW APPROPRIATE EXCEPTION */
    }

    Eigen::Vector3f measurement;
    measurement.Zero();

    timestamp current_time = this->sim->accelerometer_take_measurement(&measurement);
    this->update_poll_time(current_time);

    this->current_vector_value.time_taken = current_time;
    this->current_vector_value.vec = measurement;

    return this->current_vector_value;
}
