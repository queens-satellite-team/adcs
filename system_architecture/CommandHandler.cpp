#include "CommandExecuter.h"
#include <string>

using namespace std;

class CommandHandler {
  //------ attributes ------
private:
  enum Commands { BOOT = 1, INTERRUPTS = 2, POINT = 3, TLE = 4, POWERMODE = 5 };

  // if the value is 0x0 then we haven't gotten a command
private:
  int command_code = 0;

public:
  void get_command_code(int x) {
    // sets the command_code variable from hardware signal
    // dummy value until hardware interaction is coded
    command_code = x;
  }

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

    }; // end switch

  }; // end command_handler()
};
