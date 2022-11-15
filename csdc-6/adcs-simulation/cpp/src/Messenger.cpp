/**
 * @file    Messenger.cpp
 *
 * @details This file implements the Messenger class as defined in Messenger.hpp.
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-11-09
 *
**/

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <tgmath.h>

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

void Messenger::send_error(std::string msg)
{
    if (msg.empty())
    {
        // This may need to be a different exception.
        throw invalid_message("Error to send to UI is empty.");
    }

    // This can be formatted nicely later.
    std::cerr << text_colour.red << "ERROR: " << msg << text_colour.reset << std::endl;

    return;
}

void Messenger::start_new_sim(uint32_t num_reaction_wheels)
{
    write_cout_header(num_reaction_wheels);
    write_csv_header(num_reaction_wheels);

    return;
}

void Messenger::write_cout_header(uint32_t num_reaction_wheels)
{
    std::cout << text_colour.magenta << "Time" << "\t\t";
    std::cout << "Sat tx, Sat ty, Sat tz;\t\t" << "Sat wx, Sat wy, Sat wz;\t\t" << "Sat ax, Sat ay, Sat az;\t\t";
    std::cout << "Accel x, Accel y, Accel z;\t";// << "Gyro x, Gyro y, Gyro z;\t\t";
    for (uint32_t i = 0; i < num_reaction_wheels; i++)
    {
        std::cout << "RW " << i+1 << " O, RW " << i+1 << " a;";
        if (i < num_reaction_wheels - 1)
        {
            std::cout << "\t\t";
        }
    }
    std::cout << text_colour.reset << std::endl;

    return;
}

void Messenger::write_csv_header(uint32_t num_reaction_wheels)
{
    /* Determine output file name */
    std::string csv_path = this->default_csv_path;
    std::string suffix = "";
    uint32_t suffix_num = 0;

    if (!std::filesystem::exists(csv_path))
    {
        std::filesystem::create_directory(csv_path);
    }
    csv_path += this->default_csv_name;

    /* search for file if it exists, increment if it does */
    while(std::filesystem::exists(csv_path + suffix + this->csv_ext))
    {
        if (UINT32_MAX <= suffix_num)
        {
            this->output_file_path_string = "-1";
            send_error("Unable to create output CSV, too many files exist.");
        }
        suffix_num++;
        suffix = std::to_string(suffix_num);
    }

    if ("-1" != this->output_file_path_string)
    {
        this->output_file_path_string = csv_path + suffix + this->csv_ext;
        std::ofstream output_file;
        output_file.open(output_file_path_string);
        if (output_file.is_open())
        {
            output_file << "Time,Satellite theta x,Satellite theta y,Satellite theta z,Satellite Omega x,Satellite Omega y,Satellite Omega z,";
            output_file << "Satellite alpha x,Satellite alpha y,Satellite alpha z,Accelerometer x,Accelerometer y,Accelerometer z,";//Gyro x,Gyro y,Gyro z,";
            for (uint32_t i = 0; i < num_reaction_wheels; i++)
            {
                output_file << "Reaction wheel " << i << " Omega,Reaction wheel " << i << " alpha,";
            }
            output_file << std::endl;
            output_file.close();
        }
        else
        {
            send_error("Unable to open file " + output_file_path_string);
        }
    }

    return;
}


void Messenger::update_simulation_state(sim_config state, timestamp time)
{
    terminal_write_count++;
    csv_write_count++;

    // Do we need any checks on state?

    if ( (!silent_sim_prints) &&
         (terminal_print_rate <= terminal_write_count) )
    {
        std::cout << text_colour.reset << time.pretty_string() << "\t" << std::setprecision(4) << std::fixed;
        std::cout << state.satellite.theta_b.x() << ", " << state.satellite.theta_b.y() << ", " << state.satellite.theta_b.z() << ";\t\t";
        std::cout << state.satellite.omega_b.x() << ", " << state.satellite.omega_b.y() << ", " << state.satellite.omega_b.z() << ";\t\t";
        std::cout << state.satellite.alpha_b.x() << ", " << state.satellite.alpha_b.y() << ", " << state.satellite.alpha_b.z() << ";\t";

        std::cout << state.accelerometer.measurement.x() << ", " << state.accelerometer.measurement.y() << ", " << state.accelerometer.measurement.z() << ";\t";
        // std::cout << state.gyroscope.measurement.x()     << ", " << state.gyroscope.measurement.y()     << ", " << state.gyroscope.measurement.z() << ";";

        for (uint32_t i = 0; i < state.reaction_wheels.size(); i++)
        {   
            std::cout << "\t" << state.reaction_wheels.at(i).omega << ", " << state.reaction_wheels.at(i).alpha << ";";

            if (i < state.reaction_wheels.size() - 1)
            {
                std::cout << "\t";
            }
        }
        std::cout << std::endl;

        terminal_write_count = 0;
    }

    if (csv_print_rate <= csv_write_count)
    {
        this->append_csv_output(state, time);
        csv_write_count = 0;
    }


    return;
}

void Messenger::append_csv_output(sim_config state, timestamp time)
{
    /* don't bother trying to append if the file was never started. */
    if ("-1" != this->output_file_path_string)
    {
        std::ofstream output_file;

        try
        {
            output_file.open(output_file_path_string, std::fstream::out | std::fstream::app);
        }
        catch(const std::ios_base::failure& e)
        {
            send_error(e.what());
        }

        if (output_file.is_open())
        {
            output_file << (float)time << ",";
            output_file << state.satellite.theta_b.x() << "," << state.satellite.theta_b.y() << "," << state.satellite.theta_b.z() << ",";
            output_file << state.satellite.omega_b.x() << "," << state.satellite.omega_b.y() << "," << state.satellite.omega_b.z() << ",";
            output_file << state.satellite.alpha_b.x() << "," << state.satellite.alpha_b.y() << "," << state.satellite.alpha_b.z() << ",";

            output_file << state.accelerometer.measurement.x() << "," << state.accelerometer.measurement.y() << "," << state.accelerometer.measurement.z() << ",";
            // output_file << state.gyroscope.measurement.x()     << "," << state.gyroscope.measurement.y()     << "," << state.gyroscope.measurement.z()     << ",";

            for (uint32_t i = 0; i < state.reaction_wheels.size(); i++)
            {
                output_file << state.reaction_wheels.at(i).omega << "," << state.reaction_wheels.at(i).alpha << ",";
            }
            output_file << std::endl;

            // Close file
            output_file.close();
        }
        else
        {
            send_error("Unable to open file " + output_file_path_string);
        }
    }
 
    return;
}

void Messenger::clean_csv_files()
{
    if (std::filesystem::exists(default_csv_path))
    {
        std::filesystem::remove_all(default_csv_path);
    }
    else
    {
        send_message("Output files already clean.");
    }
    return;
}

void Messenger::silence_sim_prints()
{
    this->silent_sim_prints = true;
    this->send_message("printouts will be silenced.");
    return;
}

void Messenger::reset_defaults()
{
    this->silent_sim_prints   = default_silent_sim_prints;
    this->csv_print_rate      = default_csv_print_rate;
    this->terminal_print_rate = default_terminal_print_rate;
    return;
}

void Messenger::set_csv_print_rate(uint32_t csv_rate)
{
    this->csv_print_rate = csv_rate;
    this->send_message("CSV update period: " + std::to_string(this->csv_print_rate) + "ms.");
    return;
}

void Messenger::set_terminal_print_rate(uint32_t terminal_rate)
{
    this->terminal_print_rate = terminal_rate;
    this->send_message("Terminal print period: " + std::to_string(this->terminal_print_rate) + "ms.");
    return;
}
