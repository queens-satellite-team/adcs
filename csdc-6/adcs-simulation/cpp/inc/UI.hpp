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
#include <vector>
#include <functional>

#include "sim_interface.hpp"
#include "ConfigurationSingleton.hpp"

// this is considered to be very bad practice
// using namespace std;

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
    string reset  = "\033[0m";
    string red    = "\033[31m";
    string green  = "\033[32m";
    string yellow = "\033[33m";
} text_colour;

/**
 * @class   UI
 * 
 * @details This class defines a basic terminal-based user interface. The functions provided allow
 *          for future expandability to graphs, or even a more advanced GUI if desired.
**/
class UI
{
    public:
        UI();

        /**
         * @name    start_ui_loop
         * 
         * @details Contains a basic user input that gets a user input, parses the arguments into a
         *          vector of words, and passes the command to run_command for processing.
        **/
        void start_ui_loop();

        /**
         * @name    send_message
         * 
         * @details Function used by all internal processes and commands to send messages to the
         *          UI. Uniformily formats the messages for consistency and clarity.
         * 
         * @param msg message to display to the user.
        **/
        void send_message(string msg);

        /**
         * @name    send_warning
         * 
         * @details Function used by all internal processes and commands to send warnings to the
         *          UI. Uniformily formats the messages for consistency and clarity.
         * 
         * @param msg warning to display to the user.
        **/
        void send_warning(string msg);

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

    private:
        /**
         * @name    process_input_buffer
         * 
         * @details splits the user input buffer into word-arguments.
         * 
         * @param buffer the input buffer from the user.
         * 
         * @returns the arguments from the buffer, split into words. Each element of the vector is
         *          an individual argument.
        **/
        vector<string> process_input_buffer(string buffer);

        /**
         * @name    run_command
         * 
         * @details Tries to run the input command from the user. Any unknown commands will return
         *          a warning to the user.
         * 
         * @param args the user input arguments
        **/
        void run_command(vector<string> args);

        /**
         * @name    run_simulation
         * 
         * @details Input command to start the simulation. Instantiates a Simulator object with the
         *          starting and end state YAML files, and starts the simulation. Returns when the
         *          simulation is complete.
         * 
         * @param args the user input arguments. Arguments are as follows:
         *              args[0] command "start_sim"
         *              args[1] path to the initial conditions YAML file
         *              args[2] path to the "stop conditions" YAML file
        **/
        void run_simulation(vector<string> args);

        void create_sensor(const std::string &name, Simulator *sim, std::unordered_map<std::string, std::shared_ptr<Sensor>> *sensors);

        void create_actuator(const std::string &name, Simulator *sim, std::unordered_map<std::string, std::shared_ptr<Actuator>> *actuators);

        sim_config get_sim_config(Configuration &config);

        /**
         * @name    resume_simulation
         * 
         * @details Input command to resume the previous simulation where it left off. Takes an end
         *          state YAML as input, and uses the previous "final state" YAML as the new
         *          initial conditions.
         * 
         * @param args the user input arguments. Arguments are as follows:
         *              args[0] command "resume_sim"
         *              args[1] path to the "stop conditions" YAML file
        **/
        void resume_simulation(vector<string> args);

        /**
         * @name    quit
         * 
         * @details Exits the user interface, ending the session.
         * 
         * @param args the user input arguments. Arguments are as follows:
         *              args[0] command "exit"
        **/
        void quit(vector<string> args);

        /**
         * @typedef UI::*commandFunc
         * 
         * @details function pointer for internal commands. Used to map strings of commands to
         *          their function implementation.
        **/
        using commandFunc = std::function<void(std::vector<std::string>)>;

        /* Map linking each command to it's function implementation. Used by "run_command" */
        unordered_map<string, commandFunc> allowed_commands;

        /* Character used to denote user control of the terminal. */
        const string prompt_char = ">";

        /* Flag used when the terminal has been started. */
        bool terminal_active;

        /* Number of expected args for the "start_sim" command */
        const uint8_t num_run_simulation_args = 3;
        
        /* Number of expected args for the "resume_sim" command */
        const uint8_t num_resume_simulation_args = 2;

        /* Number of expected args for the "exit" command */
        const uint8_t num_exit_args = 1;

        /* Path to the YAML file describing the final state of the previous simulation run. */
        string previous_end_state_yaml;
};

/**
 * @exception invalid_ui_args
 * 
 * @details exception used to indicate that a UI function has received bad arguments from the user.
**/
class invalid_ui_args : public exception
{
    public:
        invalid_ui_args(char* msg) : message(msg) {}
        char* what()
        {
            return message;
        }
    
    private:
        char* message;
};
