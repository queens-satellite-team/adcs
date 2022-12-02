/** 
 * @file    ADCS_timer.cpp
 *
 * @details This file implements the ADCS_timer class as defined in sim_interface.
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-11-18
 *
**/

#include <Eigen/Dense>

#include "sim_interface.hpp"
#include "Simulator.hpp"

ADCS_timer::ADCS_timer(Simulator* sim)
{
    if (nullptr == sim)
    {
<<<<<<< HEAD
        /* THROW EXCEPTION**/
        throw std::invalid_argument("SIM in ADCS_timer IS INVALID BAD BOYS");
=======
        throw invalid_adcs_param("Simulator pointer is null in ADCS timer");
>>>>>>> a37556cb78d280d475d3860ff0e31914ab7c45db
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
