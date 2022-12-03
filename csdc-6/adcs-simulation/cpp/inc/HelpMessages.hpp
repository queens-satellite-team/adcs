/**
 * @file    HelpMessages.hpp
 *
 * @details This file describes the help messages for each UI command
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-12-01
 *
**/

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "adcs_exception.hpp"

/**
 * @class   HelpMessages
 *
 * @details This class describes the 
 *          
**/
namespace HelpMessages
{
    std::string get_command_help(std::string command_name);

    std::string get_generic_help();

    std::string introduction();
}

/**
 * @exception device_not_ready
 *
 * @details exception used to indicate that an adcs device has not completed it's required poll delay.
**/
class invalid_help_command : public adcs_exception
{
    public:
        invalid_help_command(const char* msg) :  adcs_exception(msg) {}
};