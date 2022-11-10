/**
 * @file    UI.cpp
 *
 * @details This file implements the UI class as defined in UI.hpp
 *
 * @authors Aidan Sheedy, Lily de Loe
 *
 * Last Edited
 * 2022-11-08
 *
**/

#include <string>
#include <vector>
#include <any>
#include <iostream>

#include "UI.hpp"
#include "Simulator.hpp"
#include "PointingModeController.hpp"
#include "ConfigurationSingleton.hpp"
#include "SensorActuatorFactory.hpp"

UI::UI()
{
    /* Full command names*/
    allowed_commands["start_sim"]   = std::bind(&UI::run_simulation,    this, std::placeholders::_1);
    allowed_commands["resume_sim"]  = std::bind(&UI::resume_simulation, this, std::placeholders::_1);
    allowed_commands["exit"]        = std::bind(&UI::quit,              this, std::placeholders::_1);
    allowed_commands["clean_out"]   = std::bind(&UI::clean_out,         this, std::placeholders::_1);

    /* Aliases */
    allowed_commands["ss"] = std::bind(&UI::run_simulation,    this, std::placeholders::_1);
    allowed_commands["rs"] = std::bind(&UI::resume_simulation, this, std::placeholders::_1);
    allowed_commands["q"]  = std::bind(&UI::quit,              this, std::placeholders::_1);
    allowed_commands["co"] = std::bind(&UI::clean_out,         this, std::placeholders::_1);
}

void UI::start_ui_loop()
{
    this->terminal_active = true;
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
            messenger.send_warning(e.what());
        }
        catch(...)
        {
            // This block catches all exceptions, and prints the error message. The user can decide
            // if it requires restarting the simulation, or exiting.
            std::exception_ptr e = std::current_exception();
            try
            {
                std::rethrow_exception(e);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }
        }
    }

    return;
}

void UI::run_simulation(std::vector<std::string> args)
{
    if (num_run_simulation_args != args.size())
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

    std::string config_yaml = args.at(1);

    Configuration &config = Configuration::GetInstance();
    if (!config.Load(config_yaml))
    {
        std::cout <<"Configuration Failed to load"<< std::endl;
    }
    // string exit_conditions_yaml = args.at(2);

    /* Empty simulator**/
    Simulator simulator(&messenger);

    /* Initialize Interface Objects**/
    std::unordered_map<std::string, std::shared_ptr<Sensor>> sensors;
    std::unordered_map<std::string, std::shared_ptr<Actuator>> actuators;

    /* Get Simulation config info**/
    simulator.init(this->get_sim_config(config));

    for (const auto &sensor : config.GetSensorConfigs())
    {
        //first is string, second is data (from map)
        this->create_sensor(sensor.first, &simulator, &sensors);
    }

    for (const auto &actuator : config.GetActuatorConfigs()) {
        //first is string, second is data (from map)
        this->create_actuator(actuator.first, &simulator, &actuators);
    }

    /* Start control code**/
    PointingModeController controller(sensors, actuators);

    controller.begin({0}); // Empty desired attitude for now

    /* This code should not be deleted - it will be necessary when the final_state_yaml is implemented.**/
    // string final_state_yaml_path;

    // if (!final_state_yaml_path.empty())
    // {
    //     /* TODO this could also check if the yaml is a valid path, but race condition again. Still,
    //      * in this case it is probably worth checking because there's no point in saving it as a
    //      * variable if it's invalid to start with.
    //     **/
    //     this->previous_end_state_yaml = final_state_yaml_path;
    // }

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
    initial_values.satellite.alpha_b = Eigen::Vector3f::Zero();
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
                initial_values.gyroscope.measurement = Eigen::Vector3f::Zero();
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
                initial_reac_values.alpha = Eigen::Vector3f::Ones();
                initial_reac_values.omega = Eigen::Vector3f::Zero(); // TODO: this should actually be an initial value?
                initial_reac_values.inertia  = reaction_config->momentOfInertia;
                initial_reac_values.position = reaction_config->position;
                initial_values.reaction_wheels.push_back(initial_reac_values);
            }
        }
    }
    return initial_values;
}

void UI::resume_simulation(std::vector<std::string> args)
{
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
