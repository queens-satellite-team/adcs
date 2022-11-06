/** 
 * @file    UI.cpp
 *
 * @details This file implements the UI class as defined in UI.hpp
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-11-05
 *
**/

#pragma once

#include <String>
#include <vector>
#include <any>
#include <iostream>

#include "UI.hpp"
#include "PointingModeController.hpp"


UI::UI()
{
    allowed_commands["start_sim"] = run_simulation;
    allowed_commands["resume_sim"] = resume_simulation;
    allowed_commands["exit"] = quit;
}

void UI::start_ui_loop()
{
    this->terminal_active = true;
    cout << prompt_char;

    /*
     * For now terminal_active remains true. Future commands may change the flag to false to break
     * from the loop gracefully.
     */
    while (terminal_active)
    {
        // get user input
        string buffer;
        getline(cin, buffer);

        vector<string> args = process_input_buffer(buffer);
        run_command(args);

        cout << prompt_char;
    }
    return;
}

vector<string> UI::process_input_buffer(string buffer)
{
    stringstream   buffer_stream(buffer);
    string         word;
    vector<string> args;

    while (buffer_stream >> word)
    {
        args.push_back(word);
    }

    return args;
}

void UI::run_command(vector<string> args)
{
    commandFunc command;
    bool valid_function = true;

    try
    {
        command = allowed_commands.at(args.at(0));
    }
    catch(const std::out_of_range& e)
    {
        this->send_warning("Command not found.");
        valid_function = false;
    }

    if (valid_function)
    {
        try
        {
            (this->*command)(args);
        }
        catch(invalid_ui_args e)
        {
            this->send_warning(e.what());
        }
        catch(...)
        {
            // This block catches all exceptions, and prints the error message. The user can decide
            // if it requires restarting the simulation, or exiting.
            exception_ptr e = current_exception();
            try
            {
                rethrow_exception(e);
            }
            catch(const exception& e)
            {
                cerr << e.what() << endl;
            }
        }
    }

    return;
}

void UI::run_simulation(vector<string> args)
{
    if (num_run_simulation_args != args.size())
    {
        throw invalid_ui_args("Invalid number of arguments.");
    }

    for (string arg : args)
    {
        if (arg.empty())
        {
            throw invalid_ui_args("Argument string empty!");
        }
    }

    string initial_state_yaml   = args.at(1);
    // string exit_conditions_yaml = args.at(2);

    /* 
     * We can check for the file path existing here, but it is a race condition. The simulator
     * should check it when it opens it and will return an exception, we can have a simple check
     * for an "invalid file path" exception when instantiating the simulator, which is probably a
     * better way of doing it.
     */

    // add try catch for above problem?
    Simulator simulator(initial_state_yaml);

    // string final_state_yaml_path;
   
    /*
     * In init, simulator should create a Control code class that has all of the sensors and 
     * actuators. Then, this function should call simulator with a function like:
     */

    simulator.begin();

    /* 
     * Again when passing the exit_conditions_yaml there's a possibility that it is an invalid
     * path. Same problem arises with it being a race condition though.
     */

    // if (!final_state_yaml_path.empty())
    // {
    //     /* TODO this could also check if the yaml is a valid path, but race condition again. Still,
    //      * in this case it is probably worth checking because there's no point in saving it as a
    //      * variable if it's invalid to start with.
    //      */
    //     this->previous_end_state_yaml = final_state_yaml_path;
    // }

    return;
}

void UI::resume_simulation(vector<string> args)
{
    if (num_resume_simulation_args != args.size())
    {
        throw invalid_ui_args("Invalid number of arguments.");
    }

    for (string arg : args)
    {
        if (arg.empty())
        {
            throw invalid_ui_args("Argument string empty!");
        }
    }

    /* Use previous final state yaml as new initial state */
    vector<string> new_args = {
        "unused",
        this->previous_end_state_yaml,
        args.at(1)
    };
    this->run_simulation(new_args);
}

void UI::quit(vector<string> args)
{
    if (num_exit_args != args.size())
    {
        throw invalid_ui_args("Invalid number of arguments.");
    }
    
    send_message("exiting." + text_colour.reset);
    exit(0);
}

void UI::send_message(string msg)
{
    if (msg.empty())
    {
        // This may need to be a different exception.
        throw invalid_ui_args("Message to send to UI is empty.");
    }

    // This can be formatted nicely later.
    cout << text_colour.green << msg << endl;
    return;
}


void UI::send_warning(string msg)
{
    if (msg.empty())
    {
        // This may need to be a different exception.
        throw invalid_ui_args("Warning to send to UI is empty.");
    }

    // This can be formatted nicely later.
    cout << text_colour.yellow << "WARNING: " << msg << endl;
    return;
}

void UI::update_simulation_state(Satellite state, timestamp time)
{
    // Do we need any checks on state?

    // for now just text dump - next step is graphs for each.
    cout << text_colour.reset << time.pretty_string() << "\t";
    cout << state.theta_b.x() << ", " << state.theta_b.y() << ", " << state.theta_b.z() << ";\t";
    cout << state.omega_b.x() << ", " << state.omega_b.y() << ", " << state.omega_b.z() << ";\t";
    cout << state.alpha_b.x() << ", " << state.alpha_b.y() << ", " << state.alpha_b.z() << endl;

    return;
}
