


#include <string>

#include "HelpMessages.hpp"
#include "CommonStructs.hpp"





namespace HelpMessages
{
    namespace
    {
        std::string generic_help_string =
        {
            "This is the QSET ADCS attitude simulator. You can pick from the following list of commands to run\n"
            "simulations, plot the results, and run various other tests:\n" +
            text_colour.yellow + 
            "    start_sim <config_yaml> <exit_yaml>\n"
            "    resume_sim\n"
            "    exit\n"
            "    clean_out\n"
            "    unit_test\n"
            "    perf_test\n"
            "    clean_plots\n\n" + 
            text_colour.reset +

            "For more info, type " + text_colour.yellow + "help <command>\n" +
            text_colour.reset
        };

        std::string introduction_string = 
        {
            "\n"
            "   ^:\n"
            "    ^7?7!^.\n"
            "       :~7\?\?!^.\n"
            "           .^!??7~:\n"
            "               .:~7?7~:.                                                         :~7:\n"
            "                    :~7\?\?!^.                                                .^!7?7~:\n"
            "                        .^!??7~:       .?5?.                            .^!??7^.\n"
            "                            .^!7J?!^:~7JGGGJ7~:                     :~7\?\?!^.\n"
            "                                :75P    GGGY  YJ7~:.           .:~7?7!:.\n"
            "                         :^.:~7?        GGG5     5YJ7!^^!: .^!??7~:\n"
            "                        ^P5             5J??         ?J?~:\n"
            "                        :P5             GGG5###        #BP:\n"
            "                        :PJ             PGGY  #####     G5\n"
            "                     :~7YP?             GGG5  ######### 55.\n"
            "                 .~7?J7^:5J      YG#    GBG5    ####### 5Y.\n"
            "             :~7\?\?!^.    5J    #&###    G#G5        ##  ?P.\n"
            "        .:!7?7~:.       .5J  B#B        PBG5  ###       75:\n"
            "    .^!??7~:             5Y             5GG5  #######   J5:\n"
            ".~7\?\?!^.                 55             5GGP   #########PPJ!^.\n"
            "^!^.                     55             GGPY     ###### 5~^!77!^.\n"
            "                         J5             5GGP &#         !5^   .^!7?7~:\n"
            "                         ?5             5GGP ######     !5!        :~7?7!^.\n"
            "                         ?5      ###### YGPG   ########  5!            .^!77!^.\n"
            "                         75  #########  JGGG    ######## Y!                .^!:\n"
            "                         !5 #########   JGGG      #  ##  Y? \n"
            "                         !5  ####      #5GPG ###         Y? \n"
            "                         ~P        #####5GGG  #######    J? \n"
            "                         ~P    ######## ?GGG   ##########YJ \n"
            "                         ^P  #########  ?BGG       ##### YJ \n"
            "                         :P ######      PGBG ##          ?J \n"
            "                         :P         ### PGBG  #####      ?5 \n"
            "                         :P     ####### JGGG   ########  YY \n"
            "                         .G ##########  7PGG    #########5Y.\n"
            "                         .G ########    5PGG        B### J5.\n"
            "                         .G           ##B5GP  ##         75.\n"
            "                          5       ######55GG   ######    ?5:\n"
            "                          Y5 ###########PPGG    #########P5^\n"
            "                          Y5#########   AGPG      #######P5^\n"
            "                          J5  ###       55P5        #    75^\n"
            "                          ?B          7~:..:.:^!         !5~\n"
            "                          J5       ~:.          .:~      Y5~\n"
            "                          7B    /?!.             .~J?    BG!\n"
            "                          ^??7?Y   Y?!          .:   5J!~!!:\n"
            "                               .:~7   ^^::::::^~75Y?!^.\n"
            "                                   .:^!J PBGG  ~:.\n"
            "                                        :YPP~\n\n"
            "  ______    ______   ________  ________         ______   _______    ______    ______\n"
            " /      \\  /      \\ |        \\|        \\       /      \\ |       \\  /      \\  /      \\\n"
            "|  $$$$$$\\|  $$$$$$\\| $$$$$$$$ \\$$$$$$$$      |  $$$$$$\\| $$$$$$$\\|  $$$$$$\\|  $$$$$$\\\n"
            "| $$  | $$| $$___\\$$| $$__       | $$         | $$__| $$| $$  | $$| $$   \\$$| $$___\\$$\n"
            "| $$  | $$ \\$$    \\ | $$  \\      | $$         | $$    $$| $$  | $$| $$       \\$$    \\\n"
            "| $$ _| $$ _\\$$$$$$\\| $$$$$      | $$         | $$$$$$$$| $$  | $$| $$   __  _\\$$$$$$\\\n"
            "| $$/ \\ $$|  \\__| $$| $$_____    | $$         | $$  | $$| $$__/ $$| $$__/  \\|  \\__| $$\n"
            " \\$$ $$ $$ \\$$    $$| $$     \\   | $$         | $$  | $$| $$    $$ \\$$    $$ \\$$    $$\n"
            "  \\$$$$$$\\  \\$$$$$$  \\$$$$$$$$    \\$$          \\$$   \\$$ \\$$$$$$$   \\$$$$$$   \\$$$$$$\n"
            "      \\$$$\n"
            "  ______   __                          __             __\n"
            " /      \\ |  \\                        |  \\           |  \\\n"
            "|  $$$$$$\\ \\$$ ______ ____   __    __ | $$  ______  _| $$_     ______    ______\n"
            "| $$___\\$$|  \\|      \\    \\ |  \\  |  \\| $$ |      \\|   $$ \\   /      \\  /      \\\n"
            " \\$$    \\ | $$| $$$$$$\\$$$$\\| $$  | $$| $$  \\$$$$$$\\\\$$$$$$  |  $$$$$$\\|  $$$$$$\\\n"
            " _\\$$$$$$\\| $$| $$ | $$ | $$| $$  | $$| $$ /      $$ | $$ __ | $$  | $$| $$   \\$$\n"
            "|  \\__| $$| $$| $$ | $$ | $$| $$__/ $$| $$|  $$$$$$$ | $$|  \\| $$__/ $$| $$\n"
            " \\$$    $$| $$| $$ | $$ | $$ \\$$    $$| $$ \\$$    $$  \\$$  $$ \\$$    $$| $$\n"
            "  \\$$$$$$  \\$$ \\$$  \\$$  \\$$  \\$$$$$$  \\$$  \\$$$$$$$   \\$$$$   \\$$$$$$  \\$$\n\n" +
            text_colour.magenta + 
            "To start a simulation, run " + text_colour.yellow + "start_sim <config_yaml> <exit_yaml>\n" +
            text_colour.magenta +
            "For more help, type " + text_colour.yellow + "help " + text_colour.magenta + "or " + text_colour.yellow + "help <command>\n" +
            text_colour.reset
        };

        std::string start_sim_help =
        {
            text_colour.yellow + 
            "start_sim " + text_colour.reset + "(shorthand: " + text_colour.yellow + "ss" + text_colour.reset + ")\n\n"
            "Starts a simulation with the provided files as configuration files. It will run until either a) the\n"
            "provided timeout is reached, or b) the controller has reached the desired pointing state.\n\n"
            "Mandatory arguments:\n" +
            text_colour.yellow +
            "    <config_yaml>    " + text_colour.reset + "The path to the config yaml. This yaml describes the initial state of the\n"
            "                     simulation, as well as the simulator settings. For examples, see the unit_tests folder.\n"
            "Optional arguments:\n" +
            text_colour.yellow +
            "    <exit_yaml>      " + text_colour.reset + "The path to the exit yaml. This yaml describes the exit conditions for the controller.\n"
            "                     If this flag is not provided, the simulation will run without a controller\n"
            "                     until it times out. For examples, see the unit_tests/controller/ folder.\n"
            "Flags:\n" +
            text_colour.yellow +
            "    --silent            " + text_colour.reset  + "mutes the terminal during the simulation\n"
            "      shorthand: "        + text_colour.yellow + "-s\n"
            "    --csv_rate <rate>   " + text_colour.reset  + "sets the csv print rate to the supplied rate in ms.\n"
            "      shorthand: "        + text_colour.yellow + "-c\n"
            "    --print_rate <rate> " + text_colour.reset  + "sets the terminal print rate to the supplied rate in ms.\n"
            "      shorthand: "        + text_colour.yellow + "-p\n"
            "    --silence_plots     " + text_colour.reset  + "prevents the simulation from printing to the output csv.\n"
            "      shorthand: "        + text_colour.yellow + "-sp\n" +
            text_colour.reset
        };

        std::string resume_sim_help =
        {
            text_colour.yellow + 
            "resume_sim " + text_colour.reset + "(shorthand: " + text_colour.yellow + "rs" + text_colour.reset + ")\n\n" +
            text_colour.red + 
            "NOT IMPLEMENTED\n" +
            text_colour.reset
        };

        std::string exit_help =
        {
            text_colour.yellow + 
            "exit " + text_colour.reset + "(shorthand: " + text_colour.yellow + "q" + text_colour.reset + ")\n\n"
            "Exits the simulation terminal.\n"
        };

        std::string clean_out_help =
        {
            text_colour.yellow + 
            "clean_out " + text_colour.reset + "(shorthand: " + text_colour.yellow + "co" + text_colour.reset + ")\n\n"
            "Cleans the output directory of all previous csv files. Make sure to copy any simulation runs you\n"
            "want to keep before running this command.\n"
        };

        std::string unit_test_help =
        {
            text_colour.yellow + 
            "unit_test " + text_colour.reset + "(shorthand: " + text_colour.yellow + "ut" + text_colour.reset + ")\n\n"
            "Runs a predefined set of tests to ensure the simulation is working properly. The first 6 are\n"
            "scenarios that have been calculated analytically. The results are compared against the exptected\n"
            "results and a pass/fail is assigned. The last three tests use the controller, in the following three\n"
            "scenarios:\n"
            "    1. The satellite is given an initial state of rest, and is asked to stay in that state for 600\n"
            "       seconds\n"
            "    2. The satellite is given an initial velocity, and is asked to return to it's original state.\n"
            "    3. The satellite is at rest, and is requested to change attidue by around 30 degrees.\n\n"
            "The output directory and plotting directories are cleared before running the tests, so make sure to\n"
            "save any results you want before running this test.\n"
        };

        std::string perf_test_help =
        {
            text_colour.yellow + 
            "perf_test " + text_colour.reset + "(shorthand: " + text_colour.yellow + "pt" + text_colour.reset + ")\n\n"
            "Runs a predefined set of tests in order to benchmark the efficiency of the simulator. Three tests\n"
            "are run ten times each and their run time is then averaged and displayed to the user. This should be\n"
            "used to determine how changes to the simulator effect efficiency.\n\n"
            "The output directory and plotting directories are cleared before running the tests, so make sure to\n"
            "save any results you want before running this test.\n"
        };

        std::string clean_plots_help =
        {
            text_colour.yellow + 
            "clean_plots " + text_colour.reset + "(shorthand: " + text_colour.yellow + "cp" + text_colour.reset + ")\n\n"
            "Cleans the plot directory of all previous figures. Make sure to copy any results runs you want to\n"
            "keep before running this command.\n"
        };

        std::unordered_map<std::string, std::string> command_to_help_map = 
        {
            {"start_sim",   start_sim_help},
            {"resume_sim",  resume_sim_help},
            {"exit",        exit_help},
            {"clean_out",   clean_out_help},
            {"unit_test",   unit_test_help},
            {"perf_test",   perf_test_help},
            {"clean_plots", clean_plots_help},

            {"ss",  start_sim_help},
            {"rs",  resume_sim_help},
            {"q",   exit_help},
            {"co",  clean_out_help},
            {"ut",  unit_test_help},
            {"pt",  perf_test_help},
            {"cp",  clean_plots_help},
        };
    }

    std::string get_generic_help()
    {
        return generic_help_string;
    }

    std::string introduction()
    {
        return introduction_string;
    }

    std::string get_command_help(std::string command_name)
    {
        std::string help_string;
        try
        {
            help_string = command_to_help_map.at(command_name);
        }
        catch(const std::exception& e)
        {
            throw invalid_help_command("No command of that name.");
        }
        
        
        return help_string;
    }
}
