// Copyright 2023 Hana Turcke
#ifndef EM_ARCHITECTURE_COMMANDEXECUTER_H
#include "CommandExecuter.h"
#include <string>

class CommandHandler {
private:
  enum Commands { BOOT = 1, INTERRUPTS = 2, POINT = 3, TLE = 4, POWERMODE = 5 };
  int command_code = 0; // if value is 0, no command retrieved

public:
  void get_command_code(int x) { command_code = x; }

  void handle_command() {
    CommandExecuter executer;

    switch (command_code) {
    case BOOT:
      executer.boot_command();
      break;

    case INTERRUPTS:
      executer.interrupts_command();
      break;

    case POINT:
      executer.point_command();
      break;

    case TLE:
      executer.tle_command();
      break;

    case POWERMODE:
      executer.powermode_command();
      break;

    case 0:
      printf("No command retrieved.");
      break;

    default:
      printf("Bad.");
      break;
    }
  }
};
