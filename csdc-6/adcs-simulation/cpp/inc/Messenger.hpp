/**
 * @file    UI.hpp
 *
 * @details This file describes the UI implementation. For now this is a simple command line UI
 *          with three commands:
 *              - "start_sim" starts a new simulation run with an initial state and desired output
 *                 configuration files
 *              - "restart_sim" resumes where the previous run left off but, but with a new desired
 *                 output configuration file
 *              - "exit" closes the terminal and ends the session
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-11-05
 *
**/

#pragma once

#include <string>
#include <iostream>

#include "CommonStructs.hpp"
#include "def_interface.hpp"


/**
 * @struct  text_colour
 *
 * @details structure containing the ANSI escape codes for the desired text colour.
 *
 * @property reset  resets the colour to the default
 * @property red    sets the colour to red
 * @property green  sets the colour to green
 * @property yellow sets the colour to yellow
**/
static const struct
{
    std::string reset  = "\033[0m";
    std::string red    = "\033[31m";
    std::string green  = "\033[32m";
    std::string yellow = "\033[33m";
} text_colour;

/**
 * @class   Messenger
 *
 * @details This class defines a basic terminal-based user interface. The functions provided allow
 *          for future expandability to graphs, or even a more advanced GUI if desired.
**/
class Messenger
{
    public:
        /**
         * @name    send_message
         *
         * @details Function used by all internal processes and commands to send messages to the
         *          UI. Uniformily formats the messages for consistency and clarity.
         *
         * @param msg message to display to the user.
        **/
        void send_message(std::string msg);

        /**
         * @name    send_warning
         *
         * @details Function used by all internal processes and commands to send warnings to the
         *          UI. Uniformily formats the messages for consistency and clarity.
         *
         * @param msg warning to display to the user.
        **/
        void send_warning(std::string msg);

        /**
         * @name    update_simulation_state
         *
         * @details Function used by the simulation to udpate the user on the state of the system
         *          at each time step. For now this just dumps the text directly to the terminal,
         *          but it should be improved for better clarity.
         *
         * @note    For now this does not allow for updating the udpate frequency, but this should
         *          be added.
         *
         * @param state state variables of the satellite, including angular position, velocity, and
         *              acceleration.
         * @param time  time of the update.
        **/
        void update_simulation_state(Satellite state, timestamp time);

        void prompt_char();

    private:
        /* Character used to denote user control of the terminal. */
        const std::string prompt_character = ">";

};

/**
 * @exception invalid_ui_args
 *
 * @details exception used to indicate that a UI function has received bad arguments from the user.
**/
class invalid_message : public std::exception
{
    public:
        invalid_message(char* msg) : message(msg) {}
        char* what()
        {
            return message;
        }

    private:
        char* message;
};
