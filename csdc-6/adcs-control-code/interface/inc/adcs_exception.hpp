/** @file adcs_exception.hpp
 *
 * @details This file defines the the exception class to be used by all exceptions in the simulator
 * 			and control code. This allows for easier debugging by not using the built in exception
 * 			classes.
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-11-18
 *
**/

#pragma once

#include <exception>

/**
 * @exception invalid_messagenger_param
 *
 * @details exception used to indicate that a message to the messenger is not valid.
**/
class adcs_exception : public std::exception
{
    public:
        adcs_exception(const char* msg) : msg(msg) {}
        const char* message()
        {
            return msg;
        }

    private:
        const char* msg;
};
