/**
 * @file main.cpp
 *
 * @details main file to control the adcs simulator
 *
 * @authors Lily de Loe
 *
 * Last Edited
 * 2022-10-24
 *
**/

#include <cstdio>

#include "UI.hpp"

int main(int argc, char **argv) {
    UI ui;

    ui.start_ui_loop();
    return 0;
}
