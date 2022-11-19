/**
 * @file    Messenger.hpp
 *
 * @details This file describes the functionality of the internal messaging from the simulation
 *          to the user.
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
#include <fstream>

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
    std::string reset   = "\033[0m";
    std::string red     = "\033[31m";
    std::string green   = "\033[32m";
    std::string yellow  = "\033[33m";
    std::string magenta = "\033[35m";
    std::string cyan    = "\033[36m";
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
         * @param colour the colour used for the text. Should be from the text_colour struct.
        **/
        void send_message(std::string msg, std::string colour = text_colour.reset);

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
         * @name    send_error
         *
         * @details Function used by all internal processes and commands to send errors to the
         *          UI. Uniformily formats the messages for consistency and clarity.
         *
         * @param msg error to display to the user.
        **/
        void send_error(std::string msg);

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
        void update_simulation_state(sim_config state, timestamp time);

        /**
         * @name    prompt_char
         *
         * @details Function to tell the messenger to print the prompt cahracter.
        **/
        inline void prompt_char()
        {
            std::cout << text_colour.reset << prompt_character;
        }

        /**
         * @name    start_new_sim
         *
         * @details prints a header with columns for each simulation property, and starts a csv
         *          file with the same header information.
         * 
         * @param num_reaction_wheels number of reaction wheels in the run, used for the header.
        **/
        void start_new_sim(uint32_t num_reaction_wheels);

        /**
         * @name    clean_csv_files
         *
         * @details clears the output directory of all files.
        **/
        void clean_csv_files();

        /**
         * @name    silence_sim_prints
         *
         * @details silences all terminal prints during the simulation run
        **/
        void silence_sim_prints();

        /**
         * @name    reset_defaults
         *
         * @details resets all settings to default values
        **/
        void reset_defaults();

        /**
         * @name    set_csv_print_rate
         *
         * @details sets the print rate to the csv file in terms of simulation time.
         * 
         * @param   csv_rate [uint32_t] the timestep in ms that the csv will be updated
        **/
        void set_csv_print_rate(uint32_t csv_rate);

        /**
         * @name    set_terminal_print_rate
         *
         * @details sets the print rate to the terminal in terms of simulation time.
         *
         * @param   csv_rate [uint32_t] the timestep in ms that the terminal will be updated
        **/
        void set_terminal_print_rate(uint32_t terminal_rate);


        /**
        * @name get_output_file_path_string
        * @return the string name for the output csv
        *
        * @details getter for filepath to the output csv
        **/
        inline const std::string &get_output_file_path_string() {
            return output_file_path_string;
        };
        /**
         * @name    get_default_csv_output_file
         *
         * @returns the total file path to the default csv output
        **/
        inline std::string get_default_csv_output_file()
        {
            return (default_csv_path + default_csv_name + csv_ext);
        }

        /**
         * @name    get_default_csv_output_path
         *
         * @returns the path to the default csv output
        **/
        inline std::string get_default_csv_output_path()
        {
            return default_csv_path;
        }

        /**
         * @name    set_terminal_print_rate
         *
         * @details sets the print rate to the terminal in terms of simulation time.
         *
         * @param   csv_rate [uint32_t] the timestep in ms that the terminal will be updated
        **/
        void silence_csv();

        /**
         * @name    close_open_csv
         *
         * @details closes the CSV file if open.
        **/
        void close_open_csv();

    private:
        /**
         * @name    append_csv_output
         *
         * @details appends a simulation state to the end of the csv output file.
        **/
        void append_csv_output(sim_config state, timestamp time);

        void write_cout_header(uint32_t num_reaction_wheels);

        void write_csv_header(uint32_t num_reaction_wheels);

    private:
        /* Character used to denote user control of the terminal.**/
        const std::string prompt_character = ">";

        /* Default csv file path */
        const std::string default_csv_path = "output/";

        /* default name of the output file */
        const std::string default_csv_name = "sim_out";

        /* extension of csv files */
        const std::string csv_ext = ".csv";

        /* default state of the terminal prints */
        const bool default_silent_sim_prints = false;

        /* default state of the csv prints */
        const bool default_silent_csv_prints = false;

        /* default print rate to the csv file in ms */
        const uint32_t default_csv_print_rate = 1;

        /* default print rate to the terminal in ms */
        const uint32_t default_terminal_print_rate = 10;

        /* full string of the output path */
        std::string output_file_path_string = "";

        /* state of the terminal prints */
        bool silent_sim_prints = false;

        /* state of the terminal prints */
        bool silent_csv_prints = false;

        /* print rate to the csv file in ms */
        uint32_t csv_print_rate = 1;

        /* print rate to the terminal in ms */
        uint32_t terminal_print_rate = 10;

        /**
         * number of attempts to write to csv. Each count corresponds to 1 ms, and gets reset when
         * it reaches csv_print_rate
        **/
        uint32_t csv_write_count = 0;

        /**
         * number of attempts to write to the terminal. Each count corresponds to 1 ms, and gets
         * reset when it reaches csv_print_rate
        **/
        uint32_t terminal_write_count = 0;

        /* CSV file to write logs to */
        std::ofstream open_output_file;

};

/**
 * @exception invalid_message
 *
 * @details exception used to indicate that a message to the messenger is not valid.
**/
class invalid_message : public std::exception
{
    public:
        invalid_message(const char* msg) : message(msg) {}
        const char* what()
        {
            return message;
        }

    private:
        const char* message;
};
