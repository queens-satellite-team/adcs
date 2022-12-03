/**
 * @file    UI.cpp
 *
 * @details This file implements the UI class as defined in UI.hpp
 *
 * @authors Aidan Sheedy, Lily de Loe
 *
 * Last Edited
 * 2022-11-19
 *
**/

#include <string>
#include <vector>
#include <any>
#include <iostream>

#include <sstream>
#include <fstream>
#include <filesystem>
#include <chrono>

#include <unistd.h> 
#include <sys/wait.h>

#include "Python.h"
#include "UI.hpp"
#include "Simulator.hpp"
#include "PointingModeController.hpp"
#include "ConfigurationSingleton.hpp"
#include "SensorActuatorFactory.hpp"
#include "DummyController.hpp"

UI::UI()
{
    /* Full command names*/
    allowed_commands["start_sim"]   = std::bind(&UI::run_simulation,    this, std::placeholders::_1);
    allowed_commands["resume_sim"]  = std::bind(&UI::resume_simulation, this, std::placeholders::_1);
    allowed_commands["exit"]        = std::bind(&UI::quit,              this, std::placeholders::_1);
    allowed_commands["clean_out"]   = std::bind(&UI::clean_out,         this, std::placeholders::_1);
    allowed_commands["unit_test"]   = std::bind(&UI::run_unit_tests,    this, std::placeholders::_1);
    allowed_commands["perf_test"]   = std::bind(&UI::run_perf_tests,    this, std::placeholders::_1);
    allowed_commands["clean_plots"] = std::bind(&UI::clean_plots,       this, std::placeholders::_1);
    allowed_commands["help"]        = std::bind(&UI::help,              this, std::placeholders::_1);

    /* Aliases */
    allowed_commands["ss"] = std::bind(&UI::run_simulation,    this, std::placeholders::_1);
    allowed_commands["rs"] = std::bind(&UI::resume_simulation, this, std::placeholders::_1);
    allowed_commands["q"]  = std::bind(&UI::quit,              this, std::placeholders::_1);
    allowed_commands["co"] = std::bind(&UI::clean_out,         this, std::placeholders::_1);
    allowed_commands["ut"] = std::bind(&UI::run_unit_tests,    this, std::placeholders::_1);
    allowed_commands["pt"] = std::bind(&UI::run_perf_tests,    this, std::placeholders::_1);
    allowed_commands["cp"] = std::bind(&UI::clean_plots,       this, std::placeholders::_1);
}

void UI::help(std::vector<std::string> args)
{
    if ( (max_help_args < args.size()) ||
         (min_help_args > args.size()) )
    {
        throw invalid_ui_args("Invalid number of parameters.");
    }

    if (min_help_args == args.size())
    {
        messenger.send_message(HelpMessages::get_generic_help());
    }
    else
    {
        messenger.send_message(HelpMessages::get_command_help(args.at(1)));
    }
    
    return;
}

void UI::start_ui_loop()
{
    this->terminal_active = true;
    messenger.send_message(HelpMessages::introduction());
    messenger.prompt_char();

    /**
     * For now terminal_active remains true. Future commands may change the flag to false to break
     * from the loop gracefully.
    **/
    while (terminal_active)
    {
        // get user input
        std::string buffer;
        std::getline(std::cin, buffer);

        std::vector<std::string> args = process_input_buffer(buffer);
        run_command(args);

        messenger.prompt_char();
    }
    return;
}

std::vector<std::string> UI::process_input_buffer(std::string buffer)
{
    std::stringstream           buffer_stream(buffer);
    std::string                 word;
    std::vector<std::string>    args;

    while (buffer_stream >> word)
    {
        args.push_back(word);
    }

    return args;
}

void UI::run_command(std::vector<std::string> args)
{
    commandFunc command;
    bool valid_function = true;

    try
    {
        command = allowed_commands.at(args.at(0));
    }
    catch(const std::out_of_range& e)
    {
        messenger.send_warning("Command not found.");
        valid_function = false;
    }

    if (valid_function)
    {
        try
        {
            // (this->*command)(args); // code in modern C++
            command(args);
        }
        catch(invalid_ui_args& e)
        {
            messenger.send_warning(e.message());
        }
        catch(adcs_exception &e)
        {
            messenger.send_error(e.message());
        }
    }

    return;
}

void UI::run_simulation(std::vector<std::string> args)
{
    this->parse_run_sim_args(args);

    Configuration &config = Configuration::GetInstance();

    if (!config.Load(this->config_yaml_path))
    {
        throw invalid_ui_args("Configuration failed to load");
    }
    else
    {
        /* Empty simulator */
        Simulator simulator(&messenger);

        /* Initialize Interface Objects */
        std::unordered_map<std::string, std::shared_ptr<Sensor>> sensors;
        std::unordered_map<std::string, std::shared_ptr<Actuator>> actuators;

        /* Get Simulation config info */
        timestamp timeout(config.getTimeout(),0);
        timestamp initial_timestep = timestamp(config.GetTimestepInMilliSeconds(),0);
        if (0 == initial_timestep)
        {
            initial_timestep = timestamp(1,0);
        }

        bool variableTimestep = config.GetTimestepDecision();
        timestamp max_timestep = timestamp(0,0);
        timestamp min_timestamp = timestamp(0,0);

        if (true == variableTimestep)
        {
            max_timestep = config.GetMaxTimestep();
            min_timestamp = config.GetMinTimestep();
        }

        simulator.init(this->get_sim_config(config), timeout, initial_timestep, variableTimestep, max_timestep, min_timestamp);

        /* Timer used for control code */
        ADCS_timer timer(&simulator);

        /**
         * If 2 yaml paths are provided, initialize controller with second yaml. Otherwise,
         * initialize the dummy controller to run until the time runs out. 
        **/
        if ("" == this->exit_conditions_yaml_path)
        {
            this->messenger.send_message("No exit yaml supplied for controller. Will run sim with no controller until timeout.");

            DummyController controller(&timer);

            try
            {
                controller.begin();
                messenger.send_message("Simulation timed out before the simulation finished.");
            }
            catch (simulation_timeout &e)
            {
                messenger.send_message(e.message());

            }
        }
        else
        {
            for (const auto &sensor : config.GetSensorConfigs())
            {
                //first is string, second is data (from map)
                this->create_sensor(sensor.first, &simulator, &sensors);
            }

            for (const auto &actuator : config.GetActuatorConfigs()) {
                //first is string, second is data (from map)
                this->create_actuator(actuator.first, &simulator, &actuators);
            }

            // string exit_conditions_yaml = args.at(2);
            if (!config.load_exit_file(this->exit_conditions_yaml_path))
            {
                messenger.send_error("Exit conditions failed to load");
            }
            else
            {
                Eigen::Vector3f final_sat_position  = config.getDesiredSatellitePosition();
#if 0
                // to be implemented later
                float allowed_jitter                = config.getAllowedJitter();
                float required_accuracy             = config.getRequiredAccuracy();
                int required_hold_time              = config.getHoldTime();
#endif

                /* Start control code */
                PointingModeController controller(sensors, actuators, &timer);

                try
                {
                    timestamp ramp_time(0, 30);
                    controller.begin(final_sat_position, ramp_time);
                }
                catch (simulation_timeout &e)
                {
                    messenger.send_message(e.message());
                }
            }
        }

        /* Cleanup After simulation */
        messenger.reset_defaults();

        /**
         * @todo create final_state_yaml 
        **/ 

        /* This code should not be deleted - it will be necessary when the final_state_yaml is implemented. */
        // string final_state_yaml_path;

        // if (!final_state_yaml_path.empty())
        // {
        //     /* TODO this could also check if the yaml is a valid path, but race condition again. Still,
        //      * in this case it is probably worth checking because there's no point in saving it as a
        //      * variable if it's invalid to start with.
        //     **/
        //     this->previous_end_state_yaml = final_state_yaml_path;
        // }

        //call the python script from inside C++
        if (!silent_plots)
        {
            plot_simulation_results(messenger.get_output_file_path_string());
        }
    }

    this->reset_simulation_argument_defaults();
    return;
}

void UI::reset_simulation_argument_defaults()
{
    this->silent_plots = this->default_silent_plots;
}

void UI::plot_simulation_results(std::string csv_path_in)
{
        char* csv_path = const_cast<char*>(csv_path_in.c_str());
        char* python_path = const_cast<char*>(python_plot_file_dir.c_str());
        char* args[] = {python_path, csv_path, NULL};

        int fork_ret = fork();
        if (-1 != fork_ret)
        {
            if (0 == fork_ret)
            {
                execv(args[0], args);
            }
            else
            {
                wait(NULL);
            }
        }
        else
        {
            messenger.send_error("failed to start process.\n");
        }
}

void UI::parse_run_sim_args(std::vector<std::string> args)
{
    if ( (max_run_simulation_args < args.size()) ||
         (min_run_simulation_args > args.size()) )
    {
        throw invalid_ui_args("Invalid number of arguments.");
    }

    /* Parse arguments. First reverse the vector so that it can be used as a stack. First arg is
     * the command, so pop immediately.
     */
    std::reverse(args.begin(), args.end());
    args.pop_back();

    this->config_yaml_path = "";
    this->exit_conditions_yaml_path = "";

    /* Parse arguments */
    if ('-' == args.back().at(0))
    {
        throw invalid_ui_args("First argument must be a YAML path.");
    }
    else
    {
        this->config_yaml_path = args.back();
        args.pop_back();
    }

    if(0 < args.size())
    {
        if('-' != args.back().at(0))
        {
            this->exit_conditions_yaml_path = args.back();
            args.pop_back();
        }

        while (0 != args.size())
        {
            if ( ("--silent" == args.back()) ||
                 ("-s"       == args.back()) )
            {
                messenger.silence_sim_prints();
                args.pop_back();
            }
            else if ( ("--csv_rate" == args.back()) ||
                      ("-c"         == args.back()) )
            {
                args.pop_back();
                if (0 < args.size())
                {
                    try
                    {
                        uint32_t csv_rate = std::stoi(args.back());
                        messenger.set_csv_print_rate(csv_rate);
                    }
                    catch (std::invalid_argument &e)
                    {
                        throw invalid_ui_args("Invalid csv print rate.");
                    }
                    args.pop_back();
                }
            }
            else if ( ("--print_rate" == args.back()) ||
                      ("-p"           == args.back()) )
            {
                args.pop_back();
                if (0 < args.size())
                {
                    try
                    {
                        uint32_t csv_rate = std::stoi(args.back());
                        messenger.set_terminal_print_rate(csv_rate);
                    }
                    catch (std::invalid_argument &e)
                    {
                        throw invalid_ui_args("Invalid csv print rate.");
                    }
                    args.pop_back();
                }
            }
            else if ( ("--silence_plots" == args.back()) ||
                      ("-sp"             == args.back()))
            {
                this->silent_plots = true;
                args.pop_back();
            }
            else
            {
                throw invalid_ui_args(std::string("bad parameter: " + args.back()).c_str());
            }
        }
    }

    return;
}

void UI::create_sensor(const std::string &name, Simulator *sim,
                       std::unordered_map<std::string, std::shared_ptr<Sensor>> *sensors)
{
    if (name.empty()) {
        std::cout <<"Device name must be populated. Got " << name << std::endl;
        return;
    }

    auto sensorPtr = SensorActuatorFactory::GetSensor(name, sim);
    if (!sensorPtr) {
        std::cout << "Unknown sensor type: " << name << std::endl;
        return;
    }

    (*sensors)[name] = std::move(sensorPtr);
}

void UI::create_actuator(const std::string &name, Simulator *sim,
                         std::unordered_map<std::string, std::shared_ptr<Actuator>> *actuators) {
    if (name.empty()) {
        std::cout << "Device name must be populated. Got " << name << std::endl;
        return;
    }

    auto actPtr = SensorActuatorFactory::GetActuator(name, sim);
    if (!actPtr) {
        std::cout << "Unknown actuator type: " << name << std::endl;
        return;
    }

    (*actuators)[name] = std::move(actPtr);
}

sim_config UI::get_sim_config(Configuration &config)
{
    sim_config initial_values;
    Satellite temp;
    initial_values.satellite.alpha_b   = Eigen::Vector3f::Zero();
    initial_values.satellite.omega_b   = config.GetSatelliteVelocity();
    initial_values.satellite.theta_b   = config.GetSatellitePosition();
    initial_values.satellite.inertia_b = config.GetSatelliteMoment();

    for (const auto &sensor : config.GetSensorConfigs())
    {
        const auto & sensor_config = config.GetSensorConfig(sensor.first);
        switch(sensor_config->type)
        {
            case SensorType::Accelerometer:
                initial_values.accelerometer.position = sensor_config->position;
                initial_values.accelerometer.measurement = Eigen::Vector3f::Zero();
                break;
            case SensorType::Gyroscope:
                initial_values.gyroscope.position = sensor_config->position;
                initial_values.gyroscope.alpha = Eigen::Vector3f::Zero();
                initial_values.gyroscope.omega = Eigen::Vector3f::Zero();
                initial_values.gyroscope.theta = Eigen::Vector3f::Zero();
                break;
        }
    }

    for (const auto &actuator : config.GetActuatorConfigs()) {
        const auto & actuator_config = config.GetActuatorConfig(actuator.first);
        switch(actuator_config->type)
        {
            case ActuatorType::ReactionWheel:
            {
                const ReactionWheelConfig* reaction_config = dynamic_cast<const ReactionWheelConfig*>(actuator_config.get());
                sim_reaction_wheel initial_reac_values;
                initial_reac_values.alpha = reaction_config->acceleration;
                initial_reac_values.omega = reaction_config->velocity;
                initial_reac_values.inertia  = reaction_config->momentOfInertia;
                initial_reac_values.position = reaction_config->position;
                initial_reac_values.axis_of_rotation = reaction_config->axisOfRotation;
                initial_values.reaction_wheels.push_back(initial_reac_values);
            }
        }
    }
    return initial_values;
}

void UI::resume_simulation(std::vector<std::string> args)
{
    messenger.send_error("Command not implemented.");
    (void)args; // silences compiler warning about unused parameter
#if 0
    if (num_resume_simulation_args != args.size())
    {
        throw invalid_ui_args("Invalid number of arguments.");
    }

    for (std::string arg : args)
    {
        if (arg.empty())
        {
            throw invalid_ui_args("Argument string empty!");
        }
    }

    /* Use previous final state yaml as new initial state**/
    std::vector<std::string> new_args = {
        "unused",
        this->previous_end_state_yaml,
        args.at(1)
    };
    this->run_simulation(new_args);
#endif
}

void UI::quit(std::vector<std::string> args)
{
    if (num_exit_args != args.size())
    {
        throw invalid_ui_args("Invalid number of arguments.");
    }

    messenger.send_message("exiting.");
    exit(0);
}

void UI::clean_out(std::vector<std::string> args)
{
    if (num_exit_args != args.size())
    {
        throw invalid_ui_args("Invalid number of arguments.");
    }

    messenger.clean_csv_files();
    return;
}

void UI::clean_plots(std::vector<std::string> args)
{
    if (num_clean_plots_args != args.size())
    {
        throw invalid_ui_args("Invalid number of arguments.");
    }

    if (std::filesystem::exists(this->plot_dir))
    {
        std::filesystem::remove_all(this->plot_dir);
    }
    return;
}

void UI::run_unit_tests(std::vector<std::string> args)
{
    if (num_run_unit_tests_args != args.size())
    {
        throw invalid_ui_args("Invalid number of arguments.");
    }

    messenger.send_message("Starting unit tests", text_colour.magenta);

    this->run_no_controller_unit_tests(args);
    this->run_controller_unit_tests(args);

}

void UI::run_no_controller_unit_tests(std::vector<std::string> args)
{
    if (num_run_unit_tests_args != args.size())
    {
        throw invalid_ui_args("Invalid number of arguments.");
    }

    /* Generate arguments to pass to the simulation (yaml path is left empty) */
    std::vector<std::string> sim_args = 
    {
        "start_sim",
        "",
        "-s",
        "-sp"
    };

    std::vector<std::string> co_args = {"clean_out"};

    messenger.send_message("Controllerless tests", text_colour.cyan);

    for (uint8_t test_num = 1; test_num <= num_no_controller_unit_tests; test_num++)
    {
        /* Clean all outputs before running */
        this->clean_out(co_args);

        /* Run Simulation */
        std::string yaml_path = expected_no_controller_unit_test_dir + unit_test_name + std::to_string(test_num) + yaml_extension;
        sim_args.at(1) = yaml_path;

        messenger.send_message("Unit Test " + std::to_string(test_num) + ":", text_colour.cyan);
        this->run_simulation(sim_args);

        /* Parse the results file for accelerations */
        std::ifstream out_file(messenger.get_default_csv_output_file());
        if (!out_file.is_open())
        {
            throw invalid_ui_args("Unable to open output path to compare with expected results.");
        }

        /* Find which columns contain the accelerations*/
        std::string line;
        std::string column_name;

        std::getline(out_file, line);
        std::stringstream ss(line);

        bool alpha_column_found = false;
        int alpha_column_num = 0;

        while( std::getline(ss, column_name, ',') &&
               (!alpha_column_found) )
        {
            if (std::string::npos != column_name.find("alpha x"))
            {
                alpha_column_found = true;
                alpha_column_num--;
            }
            alpha_column_num++;
        }

        if (!alpha_column_found)
        {
            throw invalid_ui_args("Unable to find satellite acceleration column in output file.");
        }

        /* Extract the first column */
        Eigen::Vector3f first_timestep_alphas;

        std::getline(out_file, line);
        ss = std::stringstream(line);
        std::string contents;

        int column;
        for (column = 0; (column < alpha_column_num)  && std::getline(ss, contents, ','); column++) {}

        if (column != (alpha_column_num ))
        {
            throw invalid_ui_args("Data corrupted.");
        }

        for (int j = 0; (j < 3)  && std::getline(ss, contents, ','); j++)
        {
            first_timestep_alphas[j] = std::stof(contents);
        }

        out_file.close();

        /* Find expected results */
        std::ifstream expected_result_file(expected_results_dir + unit_test_name + std::to_string(test_num) + csv_extension);

        /* Ignore header line */
        std::getline(expected_result_file, line);
        std::getline(expected_result_file, line);
        ss = std::stringstream(line);

        Eigen::Vector3f expected_alphas;

        for (int j = 0; (j < 3)  && std::getline(ss, contents, ','); j++)
        {
            expected_alphas[j] = std::stof(contents);
        }

        expected_result_file.close();

        Eigen::Vector3f diff = expected_alphas - first_timestep_alphas;
        Eigen::Vector3f tollerance = {0.01,0,0};

        if (diff.isMuchSmallerThan(tollerance))
        {
            messenger.send_message("PASS\n", text_colour.green);
        }
        else
        {
            messenger.send_message("FAIL", text_colour.red);
            std::stringstream msg;
            msg << "Expected: [" << expected_alphas.x() << "," << expected_alphas.y() << "," << expected_alphas.z() << "] ";
            msg << "Actual: [" << first_timestep_alphas.x() << "," << first_timestep_alphas.y() << "," << first_timestep_alphas.z() << "] ";
            msg << std::endl;

            messenger.send_message(msg.str(), text_colour.yellow);
        }
    }
}

void UI::run_controller_unit_tests(std::vector<std::string> args)
{
    if (num_run_unit_tests_args != args.size())
    {
        throw invalid_ui_args("Invalid number of arguments.");
    }

    messenger.send_message("Controller based tests", text_colour.cyan);

    /* Clean all outputs before running */
    std::vector<std::string> co_args = {"clean_out"};
    this->clean_out(co_args);

    /**
     * Generate arguments to pass to the simulation (yaml paths left empty) 
     * csv file is written every 100 milliseconds (sim time)
     * terminal is written every 100 seconds (sim time)
    **/
    std::vector<std::string> sim_args = 
    {
        "start_sim",
        "",
        "",
        "-c",
        "100",
        "-p",
        "100000",
        "-sp"
    };

    for (uint8_t test_num = 1; test_num <= num_controller_unit_tests; test_num++)
    {
        /* setup arguments */
        std::string config_yaml_path = expected_controller_unit_test_dir + ut_controller_config_name + std::to_string(test_num) + yaml_extension;
        std::string exit_yaml_path   = expected_controller_unit_test_dir + ut_controller_exit_name   + std::to_string(test_num) + yaml_extension;

        sim_args.at(1) = config_yaml_path;
        sim_args.at(2) = exit_yaml_path;

        messenger.send_message("\nUnit Test " + std::to_string(test_num) + ":", text_colour.cyan);
        this->run_simulation(sim_args);

        std::string new_name = (messenger.get_default_csv_output_path() + controller_test_output_name + std::to_string(test_num) + csv_extension);

        std::filesystem::path output_path     = std::filesystem::current_path() / messenger.get_default_csv_output_file();
        std::filesystem::path new_output_path = std::filesystem::current_path() / new_name;

        std::filesystem::rename(output_path, new_output_path);
        this->plot_simulation_results(new_output_path.string());
    }

    return;
}

void UI::run_perf_tests(std::vector<std::string> args)
{
    if (num_perf_test_args != args.size())
    {
        throw invalid_ui_args("Invalid number of arguments.");
    }

    messenger.send_message("Running Performance tests", text_colour.cyan);

    /* Clean all outputs before running */
    std::vector<std::string> co_args = {"clean_out"};
    this->clean_out(co_args);

    /**
     * Generate arguments to pass to the simulation. Arguments are as follows:
     * 
     * args[0] - command to start sim
     * args[1] - path to config yaml file
     * args[2] - path to exit yaml file
     * args[3] - silent operation, no prints to terminal during operation
     * args[4] - request specific print regularity to the csv
     * args[5] - print regularity of 1 ms (sim time) - this is changed by some tests.
    **/
    std::vector<std::string> test_args = 
    {
        "start_sim",
        "",
        "",
        "-s",
        "-c",
        "1000",
        "-sp"
    };

    for (uint8_t test_num = 1; test_num <= 3; test_num++)//num_performance_tests; test_num++)
    {
        messenger.send_message("\nPerf Test " + std::to_string(test_num) + ":", text_colour.cyan);
        messenger.send_message(perf_test_descriptions.at(test_num-1), text_colour.cyan);
        messenger.send_message("Test will loop 10 times.", text_colour.cyan);

        uint32_t duration = 0;

        for (uint8_t iteration = 0; iteration < 10; iteration++)
        {
            // messenger.silence_csv();
            uint32_t time_start = 0;
            uint32_t time_end = 0;

            /* Tests greater than three share a yaml file */
            uint8_t yaml_number = (3 <= test_num) ? 3 : test_num;

            if (4 == test_num)
            {
                test_args.back() = "1000";
            }
            else if (5 == test_num)
            {
                test_args.back() = "1";
                messenger.silence_csv();
            }

            test_args.at(1) = perf_test_config_yaml_path + std::to_string(yaml_number) + yaml_extension;
            test_args.at(2) = perf_test_exit_yaml_path   + std::to_string(yaml_number) + yaml_extension;

            time_start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            this->run_simulation(test_args);
            time_end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

            duration += time_end - time_start;
        }
        duration /= 10;
        messenger.send_message("Average duration (ms): " + std::to_string(duration) + "\n", text_colour.yellow);
    }

    return;
}
