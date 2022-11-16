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
 * 2022-11-07
 *
**/

#pragma once

#include <string>
#include <vector>
#include <functional>

#include "sim_interface.hpp"
#include "ConfigurationSingleton.hpp"
#include "Messenger.hpp"

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
        std::vector<std::string> process_input_buffer(std::string buffer);

        /**
         * @name    run_command
         *
         * @details Tries to run the input command from the user. Any unknown commands will return
         *          a warning to the user.
         *
         * @param args the user input arguments
        **/
        void run_command(std::vector<std::string> args);

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
        void run_simulation(std::vector<std::string> args);

        /**
         * @name    parse_run_sim_args
         *
         * @details given a set of args for the simulation, parses them and applies them. Allowed
         *          args are as follows:
         *              input_yaml_path - path to the input yaml (required)
         *              exit_yaml_path  - path to the output yaml (optional)
         *              --silent        - mutes the terminal during the simulation (optional)
         *              -- csv_rate r   - sets the csv print rate to r (ms) (optional)
         *              -- print_rate r - sets the terminal print rate to r (ms) (optional)
         *
         * @param args the user input arguments. Arguments are as follows:
         *              args[0]  command "clean_out"
         *              args[1]  input_yaml_path
         *              args[2]  exit_yaml_path (optional)
         *              args[2+] optional flags
        **/
        void parse_run_sim_args(std::vector<std::string> args);

        /**
         * @name create_sensor
         *
         * @param name [string], the name of the sensor to be created
         * @param sim  [Simulator*], the simulator pointer to be used in the sensor constructor.
         * @param sensors an unordered map to populate with the sensor.
         *
         * @details creates a sensor object and populates it in an unordered map to be used by the
         *          control code.
        **/
        void create_sensor(const std::string &name, Simulator *sim, std::unordered_map<std::string, std::shared_ptr<Sensor>> *sensors);

        /**
         * @name create_actuator
         *
         * @param name [string], the name of the actuator to be created
         * @param sim  [Simulator*], the simulator pointer to be used in the actuator constructor.
         * @param actuators an unordered map to populate with the actuator.
         *
         * @details creates an actuator object and populates it in an unordered map to be used by the
         *          control code.
        **/  
        void create_actuator(const std::string &name, Simulator *sim, std::unordered_map<std::string, std::shared_ptr<Actuator>> *actuators);

        /**
         * @name get_sim_config
         * 
         * @param config configuration singleton used to get the simulation initial parameters.
         * 
         * @returns the initial configuration fo the satellite 
        **/
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
        void resume_simulation(std::vector<std::string> args);

        /**
         * @name    quit
         *
         * @details Exits the user interface, ending the session.
         *
         * @param args the user input arguments. Arguments are as follows:
         *              args[0] command "exit"
        **/
        void quit(std::vector<std::string> args);

        /**
         * @name    clean_out
         *
         * @details Cleans all output csv files
         *
         * @param args the user input arguments. Arguments are as follows:
         *              args[0] command "clean_out"
        **/
        void clean_out(std::vector<std::string> args);

        /**
         * @name    run_unit_tests
         *
         * @details Input command to run all the unit or specified unit tests.
         * 
         * @note    for now this only runs all unit tests - future functionality may be added to
         *          specify tests to run
         *
         * @param args the user input arguments. Arguments are as follows:
         *              args[0] command "resume_sim"
        **/
        void run_unit_tests(std::vector<std::string> args);

    private:
        /**
         * @typedef UI::*commandFunc
         *
         * @details function pointer for internal commands. Used to map strings of commands to
         *          their function implementation.
        **/
        using commandFunc = std::function<void(std::vector<std::string>)>;

        /* Messenger object used to send information to the user. */
        Messenger messenger;

        /* Map linking each command to it's function implementation. Used by "run_command" */
        std::unordered_map<std::string, commandFunc> allowed_commands;

        /* Flag used when the terminal has been started. */
        bool terminal_active;

        /* Max number of args for the "start_sim" command */
        const uint8_t max_run_simulation_args = 8;

        /* Min number of args for the "start_sim" command */
        const uint8_t min_run_simulation_args = 2;

        /* Number of expected args for the "resume_sim" command */
        const uint8_t num_resume_simulation_args = 2;

        /* Number of expected args for the "exit" command */
        const uint8_t num_exit_args = 1;

        /* Number of expected args for the "exit" command */
        const uint8_t num_clean_out_args = 1;

        /* Number of expected args for the "exit" command */
        const uint8_t num_run_unit_tests_args = 1;

        /* Path where yamls describing unit tests are expected */
        const std::string expected_unit_test_dir = "unit_tests/no_controller/";

        /* Path where yamls describing unit tests are expected */
        const std::string expected_results_dir = "unit_tests/no_controller/expected_results/";

        /* YAML file extension string */
        const std::string yaml_extension = ".yaml";

        /* CSV file extension string */
        const std::string csv_extension = ".csv";

        /* unit test name prefix (without the number) */
        const std::string unit_test_name = "unit_test_";

        /* number of unit tests to run */
        const uint8_t num_unit_tests = 6;

        /* Path to the YAML file describing the final state of the previous simulation run. */
        std::string previous_end_state_yaml;

        /* Path to the YAML file describing the initial state of the simulation. */
        std::string config_yaml_path = "";

        /* Path to the YAML file describing the exit state of the controller. */
        std::string exit_conditions_yaml_path = "";
};

/**
 * @exception invalid_ui_args
 *
 * @details exception used to indicate that a UI function has received bad arguments from the user.
**/
class invalid_ui_args : public std::exception
{
    public:
        invalid_ui_args(const char* msg) : message(msg) {}
        const char* what()
        {
            return message;
        }

    private:
        const char* message;
};
