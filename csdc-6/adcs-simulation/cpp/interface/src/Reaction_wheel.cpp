/** @file   Reaction_wheel
.hpp
 *
 * @details This file implements the reaction wheel definition as defined in sim_interface.hpp.
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

Reaction_wheel::Reaction_wheel(timestamp polling_time, Simulator* sim, vector<float> position, actuator_state max_vals, actuator_state min_vals, Eigen::Matrix3f inertia_matrix) : Actuator(polling_time, sim, position, max_vals, min_vals)
{
    if (Eigen::Matrix3f::Zero() == inertia_matrix)
    {
        // throw exception
    }

    this->inertia_matrix = inertia_matrix;

    return;
}

Eigen::Matrix3f Reaction_wheel::get_inertia_matrix()
{
    return this->inertia_matrix;
}
