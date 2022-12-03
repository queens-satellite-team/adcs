/** @file interface.hpp
 *
 * @details This file manages the possible versions of the interface between the control code and
 * the driver.
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-10-11
 *
**/

#pragma once

#include <def_interface.hpp>

#if SIM_INTERFACE == INTERFACE
#include <sim_interface.hpp>
#elif STM_INTERFACE == INTERFACE
#include <stm_interface.hpp>
#endif
