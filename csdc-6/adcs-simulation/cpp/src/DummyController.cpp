/**
 * @file DummyController.cpp
 *
 * @details implements the dummy controller as defined in DummyController.hpp
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-11-14
**/

#include <iostream>

#include "DummyController.hpp"

DummyController::DummyController(ADCS_timer *timer)
{
    if (nullptr == timer)
    {
        throw invalid_adcs_param("pointer to timer was null in DummyController");
    }

    this->timer = timer;
}

void DummyController::begin()
{
    timestamp time;

    while(this->timeout > time)
    {
        timer->sleep(timestamp(0,1));
    }

    return;
}
