// Copyright 2023 QSET
#ifndef SYSTEM_ARCHITECTURE_COMMANDEXECUTER_H_
#define SYSTEM_ARCHITECTURE_COMMANDEXECUTER_H_
#include <cmath>
#include <iostream>
#include <map>
#include <string>

class CommandExecuter {
public:
  void boot_command();
  void interrupts_command();
  void point_command();
  void tle_command();
  void powermode_command();
};

#endif // SYSTEM_ARCHITECTURE_COMMANDEXECUTER_H_
