/** 
 * @file    ADCS_timer.cpp
 *
 * @details This file implements the ADCS_timer class as defined in sim_interface.
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-11-02
 *
**/

#include <Eigen/Dense>

#include "sim_interface.hpp"
#include "Simulator.hpp"

ADCS_timer::ADCS_timer(Simulator* sim)
{
    if (nullptr == sim)
    {
        /* THROW EXCEPTION**/
        throw std::invalid_argument("SIM in ADCS_timer IS INVALID BAD BOYS");
    }

    this->sim = sim;
}

timestamp ADCS_timer::get_time()
{
    return this->sim->update_simulation();
}

timestamp ADCS_timer::sleep(timestamp duration)
{
    return this->sim->set_adcs_sleep(duration);
}
