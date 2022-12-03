/** 
 * @file   Gyroscope.cpp
 *
 * @details This file implements the Gyroscope class as defined in sim_interface.
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-11-18
 *
**/

#include <Eigen/Dense>

#include "sim_interface.hpp"

gyro_state Gyroscope::take_measurement()
{
    if (this->time_until_ready() > 0)
    {
        throw device_not_ready("Gyroscope not ready.");
    }

    gyro_state measurement = this->sim->gyroscope_take_measurement();
    this->update_poll_time(measurement.time_taken);

    /* commenting out for now, but may not need either of these lines. Leaving as comments until confirmed. */
    // this->current_vector_value.time_taken = current_time;
    // this->current_vector_value.vec = measurement;

    return measurement;
}
