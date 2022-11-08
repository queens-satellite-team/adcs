/**
 * @file    Messenger.cpp
 *
 * @details This file implements the Messenger class as defined in Messenger.hpp.
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-11-07
 *
**/

#pragma once

#include <string>
#include <iostream>

#include "Messenger.hpp"

void Messenger::send_message(std::string msg)
{
    if (msg.empty())
    {
        // This may need to be a different exception.
        throw invalid_message("Message to send to UI is empty.");
    }

    // This can be formatted nicely later.
    std::cout << text_colour.green << msg << text_colour.reset << std::endl;
    return;
}


void Messenger::send_warning(std::string msg)
{
    if (msg.empty())
    {
        // This may need to be a different exception.
        throw invalid_message("Warning to send to UI is empty.");
    }

    // This can be formatted nicely later.
    std::cout << text_colour.yellow << "WARNING: " << msg << text_colour.reset << std::endl;
    return;
}

void Messenger::update_simulation_state(Satellite state, timestamp time)
{
    // Do we need any checks on state?

    // for now just text dump - next step is graphs for each.
    std::cout << text_colour.reset << time.pretty_string() << "\t";
    std::cout << state.theta_b.x() << ", " << state.theta_b.y() << ", " << state.theta_b.z() << ";\t";
    std::cout << state.omega_b.x() << ", " << state.omega_b.y() << ", " << state.omega_b.z() << ";\t";
    std::cout << state.alpha_b.x() << ", " << state.alpha_b.y() << ", " << state.alpha_b.z() << std::endl;

    return;
}

void Messenger::prompt_char()
{
    std::cout << text_colour.reset << prompt_character;
}
