/**
 * @file main.cpp
 *
 * @details main file to control the adcs simulator
 *
 * @authors Lily de Loe, Aidan Sheedy
 *
 * Last Edited
 * 2022-11-07
 *
**/

#include <cstdio>

#include "UI.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    UI ui;
    ui.start_ui_loop();
    return 0;
}
