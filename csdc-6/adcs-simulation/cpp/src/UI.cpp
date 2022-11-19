/**
 * @file    UI.cpp
 *
 * @details This file implements the UI class as defined in UI.hpp
 *
 * @authors Aidan Sheedy, Lily de Loe
 *
 * Last Edited
 * 2022-11-08
 *
**/

#include <string>
#include <vector>
#include <any>
#include <iostream>
#include <unistd.h> 
#include <sys/wait.h>

#include "Python.h"

#include "UI.hpp"
#include "Simulator.hpp"
#include "PointingModeController.hpp"
#include "ConfigurationSingleton.hpp"
#include "SensorActuatorFactory.hpp"
#include "DummyController.hpp"

UI::UI()
{
    /* Full command names*/
    allowed_commands["start_sim"]   = std::bind(&UI::run_simulation,    this, std::placeholders::_1);
    allowed_commands["resume_sim"]  = std::bind(&UI::resume_simulation, this, std::placeholders::_1);
    allowed_commands["exit"]        = std::bind(&UI::quit,              this, std::placeholders::_1);
    allowed_commands["clean_out"]   = std::bind(&UI::clean_out,         this, std::placeholders::_1);

    /* Aliases */
    allowed_commands["ss"] = std::bind(&UI::run_simulation,    this, std::placeholders::_1);
    allowed_commands["rs"] = std::bind(&UI::resume_simulation, this, std::placeholders::_1);
    allowed_commands["q"]  = std::bind(&UI::quit,              this, std::placeholders::_1);
    allowed_commands["co"] = std::bind(&UI::clean_out,         this, std::placeholders::_1);
}

void UI::start_ui_loop()
{
    this->terminal_active = true;
    messenger.prompt_char();

    /**
     * For now terminal_active remains true. Future commands may change the flag to false to break
     * from the loop gracefully.
    **/
    while (terminal_active)
    {
        // get user input
        std::string buffer;
        std::getline(std::cin, buffer);

        std::vector<std::string> args = process_input_buffer(buffer);
        run_command(args);

        messenger.prompt_char();
    }
    return;
}

std::vector<std::string> UI::process_input_buffer(std::string buffer)
{
    std::stringstream           buffer_stream(buffer);
    std::string                 word;
    std::vector<std::string>    args;

    while (buffer_stream >> word)
    {
        args.push_back(word);
    }

    return args;
}

void UI::run_command(std::vector<std::string> args)
{
    commandFunc command;
    bool valid_function = true;

    try
    {
        command = allowed_commands.at(args.at(0));
    }
    catch(const std::out_of_range& e)
    {
        messenger.send_warning("Command not found.");
        valid_function = false;
    }

    if (valid_function)
    {
        try
        {
            // (this->*command)(args); // code in modern C++
            command(args);
        }
        catch(invalid_ui_args& e)
        {
            messenger.send_warning(e.what());
        }
        catch(...)
        {
            // This block catches all exceptions, and prints the error message. The user can decide
            // if it requires restarting the simulation, or exiting.
            std::exception_ptr e = std::current_exception();
            try
            {
                std::rethrow_exception(e);
            }
            catch(const std::exception& e)
            {
                messenger.send_error(e.what());
            }
        }
    }

    return;
}

void UI::run_simulation(std::vector<std::string> args)
{
    this->parse_run_sim_args(args);

    Configuration &config = Configuration::GetInstance();

    if (!config.Load(this->config_yaml_path))
    {
        messenger.send_error("Configuration failed to load");
    }
    else
    {
        /* Empty simulator */
        Simulator simulator(&messenger);

        /* Initialize Interface Objects */
        std::unordered_map<std::string, std::shared_ptr<Sensor>> sensors;
        std::unordered_map<std::string, std::shared_ptr<Actuator>> actuators;

        /* Get Simulation config info */
        timestamp timeout(config.getTimeout(),0);
        simulator.init(this->get_sim_config(config), timeout);

        /* Timer used for control code */
        ADCS_timer timer(&simulator);

        /**
         * If 2 yaml paths are provided, initialize controller with second yaml. Otherwise,
         * initialize the dummy controller to run until the time runs out. 
        **/
        if ("" == this->exit_conditions_yaml_path)
        {
            this->messenger.send_message("No exit yaml supplied for controller. Will run sim with no controller until timeout.");

            DummyController controller(&timer);

            try
            {
                controller.begin();
                messenger.send_message("Simulation timed out before the simulation finished.");
            }
            catch (simulation_timeout &e)
            {
                messenger.send_message(e.what());

            }
        }
        else
        {
            for (const auto &sensor : config.GetSensorConfigs())
            {
                //first is string, second is data (from map)
                this->create_sensor(sensor.first, &simulator, &sensors);
            }

            for (const auto &actuator : config.GetActuatorConfigs()) {
                //first is string, second is data (from map)
                this->create_actuator(actuator.first, &simulator, &actuators);
            }

            // string exit_conditions_yaml = args.at(2);
            if (!config.load_exit_file(this->exit_conditions_yaml_path))
            {
                messenger.send_error("Exit conditions failed to load");
            }
            else
            {
                Eigen::Vector3f final_sat_position  = config.getDesiredSatellitePosition();
#if 0
                // to be implemented later
                float allowed_jitter                = config.getAllowedJitter();
                float required_accuracy             = config.getRequiredAccuracy();
                int required_hold_time              = config.getHoldTime();
#endif

                /* Start control code */
                PointingModeController controller(sensors, actuators, &timer);

                try
                {
                    controller.begin(final_sat_position);
                }
                catch (simulation_timeout &e)
                {
                    messenger.send_message(e.what());
                }
            }
        }

        /* Cleanup After simulation */
        messenger.reset_defaults();

        /**
         * @todo create final_state_yaml 
        **/ 

        /* This code should not be deleted - it will be necessary when the final_state_yaml is implemented. */
        // string final_state_yaml_path;

        // if (!final_state_yaml_path.empty())
        // {
        //     /* TODO this could also check if the yaml is a valid path, but race condition again. Still,
        //      * in this case it is probably worth checking because there's no point in saving it as a
        //      * variable if it's invalid to start with.
        //     **/
        //     this->previous_end_state_yaml = final_state_yaml_path;
        // }

        //call the python script from inside C++
        char* csv_path = const_cast<char*>(messenger.get_output_file_path_string().c_str());
        std::cout << csv_path << std::endl;
        // const char* helpme = csv_path.c_str();
        char* args[] = {csv_path, NULL};

        int fork_ret = fork();
        if (-1 != fork_ret)
        {
            if (0 == fork_ret)
            {
                std::cout << "starting execv" << std::endl;
                int execv_ret = execv("./results_visualization.py", args);
                std::cout << "child done " << execv_ret << std::endl;
                int errvalue = errno;
                std::cout << "errno " << errvalue << std::endl;
                std::cout << "meaning " << strerror(errvalue) << std::endl;
            }
            else
            {
                std::cout << "parent Path" << std::endl;
                wait(NULL);
                std::cout << "parent done done" << std::endl;
            }
        }
        else
        {
            messenger.send_error("failed to start process.\n");
        }

        

        

        //THIS PART WORKED BTW///////////
        //char filename[] = "results_visualization_old.py";
	    //FILE* fp;
	    //Py_Initialize();

	    //fp = _Py_fopen(filename, "r");
	    //PyRun_SimpleFile(fp, filename);

        //Py_Finalize();
        /////////////////////////////////


        /**
        std::string filename = "results_visualization";
        //char fnc[] = "plot_results";
        //Run a python function
        PyObject *pName, *pModule, *pFunc, *pArgs; //*pValue;
        std::cout << "PyObject" << std::endl;
        pName = PyUnicode_FromString(filename.c_str()); //PyUnicode_FromString("results_visualization"); //(char*)"results_visualization");
        std::cout << "pName" << std::endl;
        if(pName == NULL) {
            std::cout << "agh" << std::endl;
        }
        //pModule = PyImport_Import(pName);
        pModule = PyImport_Import(pName);
        std::cout << pModule->ob_type << std::endl;
        if (pModule == nullptr) {
            std::cout << "error importing" << std::endl;
        } 
        pFunc = PyObject_GetAttrString(pModule,"results_visualization");
        std::cout << pFunc << std::endl;
        if (pFunc == NULL) {
            std::cout << "ahhhh" << std::endl;
        } 
        std::cout << "pFunc" << std::endl;
        if (pFunc && PyCallable_Check(pFunc)) {
            std::cout << "passed" << std::endl;
            pArgs = PyTuple_Pack(1, csv_path);
            PyObject_CallObject(pFunc, NULL); //pArgs);
        }else{
            std::cout << "sadness" << std::endl;
        }

        //auto result = _PyUnicode_AsString(pValue);
        //std::cout << result << std::endl;
        **/
	    //Py_Finalize();



    }
    return;
}

void UI::parse_run_sim_args(std::vector<std::string> args)
{
    if ( (max_run_simulation_args < args.size()) ||
         (min_run_simulation_args > args.size()) )
    {
        throw invalid_ui_args("Invalid number of arguments.");
    }

    /* Parse arguments. First reverse the vector so that it can be used as a stack. First arg is
     * the command, so pop immediately.
     */
    std::reverse(args.begin(), args.end());
    args.pop_back();

    this->config_yaml_path = "";
    this->exit_conditions_yaml_path = "";

    /* Parse arguments */
    if ('-' == args.back().at(0))
    {
        throw invalid_ui_args("First argument must be a YAML path.");
    }
    else
    {
        this->config_yaml_path = args.back();
        args.pop_back();
    }

    if(0 < args.size())
    {
        if('-' != args.back().at(0))
        {
            this->exit_conditions_yaml_path = args.back();
            args.pop_back();
        }

        while (0 != args.size())
        {
            if ( ("--silent" == args.back()) ||
                ("-s"       == args.back()) )
            {
                messenger.silence_sim_prints();
                args.pop_back();
            }
            else if ( ("--csv_rate" == args.back()) ||
                    ("-c"         == args.back()) )
            {
                args.pop_back();
                if (0 < args.size())
                {
                    try
                    {
                        uint32_t csv_rate = std::stoi(args.back());
                        messenger.set_csv_print_rate(csv_rate);
                    }
                    catch (std::invalid_argument &e)
                    {
                        throw invalid_ui_args("Invalid csv print rate.");
                    }
                    args.pop_back();
                }
            }
            else if ( ("--print_rate" == args.back()) ||
                    ("-p"           == args.back()) )
            {
                args.pop_back();
                if (0 < args.size())
                {
                    try
                    {
                        uint32_t csv_rate = std::stoi(args.back());
                        messenger.set_terminal_print_rate(csv_rate);
                    }
                    catch (std::invalid_argument &e)
                    {
                        throw invalid_ui_args("Invalid csv print rate.");
                    }
                    args.pop_back();
                }
            }
            else
            {
                throw invalid_ui_args(std::string("bad parameter: " + args.back()).c_str());
            }
        }
    }

    return;
}

void UI::create_sensor(const std::string &name, Simulator *sim,
                       std::unordered_map<std::string, std::shared_ptr<Sensor>> *sensors)
{
    if (name.empty()) {
        std::cout <<"Device name must be populated. Got " << name << std::endl;
        return;
    }

    auto sensorPtr = SensorActuatorFactory::GetSensor(name, sim);
    if (!sensorPtr) {
        std::cout << "Unknown sensor type: " << name << std::endl;
        return;
    }

    (*sensors)[name] = std::move(sensorPtr);
}

void UI::create_actuator(const std::string &name, Simulator *sim,
                         std::unordered_map<std::string, std::shared_ptr<Actuator>> *actuators) {
    if (name.empty()) {
        std::cout << "Device name must be populated. Got " << name << std::endl;
        return;
    }

    auto actPtr = SensorActuatorFactory::GetActuator(name, sim);
    if (!actPtr) {
        std::cout << "Unknown actuator type: " << name << std::endl;
        return;
    }

    (*actuators)[name] = std::move(actPtr);
}

sim_config UI::get_sim_config(Configuration &config)
{
    sim_config initial_values;
    Satellite temp;
    initial_values.satellite.alpha_b   = Eigen::Vector3f::Zero();
    initial_values.satellite.omega_b   = config.GetSatelliteVelocity();
    initial_values.satellite.theta_b   = config.GetSatellitePosition();
    initial_values.satellite.inertia_b = config.GetSatelliteMoment();

    for (const auto &sensor : config.GetSensorConfigs())
    {
        const auto & sensor_config = config.GetSensorConfig(sensor.first);
        switch(sensor_config->type)
        {
            case SensorType::Accelerometer:
                initial_values.accelerometer.position = sensor_config->position;
                initial_values.accelerometer.measurement = Eigen::Vector3f::Zero();
                break;
            case SensorType::Gyroscope:
                initial_values.gyroscope.position = sensor_config->position;
                initial_values.gyroscope.alpha = Eigen::Vector3f::Zero();
                initial_values.gyroscope.omega = Eigen::Vector3f::Zero();
                initial_values.gyroscope.theta = Eigen::Vector3f::Zero();
                break;
        }
    }

    for (const auto &actuator : config.GetActuatorConfigs()) {
        const auto & actuator_config = config.GetActuatorConfig(actuator.first);
        switch(actuator_config->type)
        {
            case ActuatorType::ReactionWheel:
            {
                const ReactionWheelConfig* reaction_config = dynamic_cast<const ReactionWheelConfig*>(actuator_config.get());
                sim_reaction_wheel initial_reac_values;
                initial_reac_values.alpha = reaction_config->acceleration;
                initial_reac_values.omega = reaction_config->velocity;
                initial_reac_values.inertia  = reaction_config->momentOfInertia;
                initial_reac_values.position = reaction_config->position;
                initial_reac_values.axis_of_rotation = reaction_config->axisOfRotation;
                initial_values.reaction_wheels.push_back(initial_reac_values);
            }
        }
    }
    return initial_values;
}

void UI::resume_simulation(std::vector<std::string> args)
{
    messenger.send_error("Command not implemented.");
    (void)args; // silences compiler warning about unused parameter
#if 0
    if (num_resume_simulation_args != args.size())
    {
        throw invalid_ui_args("Invalid number of arguments.");
    }

    for (std::string arg : args)
    {
        if (arg.empty())
        {
            throw invalid_ui_args("Argument string empty!");
        }
    }

    /* Use previous final state yaml as new initial state**/
    std::vector<std::string> new_args = {
        "unused",
        this->previous_end_state_yaml,
        args.at(1)
    };
    this->run_simulation(new_args);
#endif
}

void UI::quit(std::vector<std::string> args)
{
    if (num_exit_args != args.size())
    {
        throw invalid_ui_args("Invalid number of arguments.");
    }

    messenger.send_message("exiting.");
    exit(0);
}

void UI::clean_out(std::vector<std::string> args)
{
    if (num_exit_args != args.size())
    {
        throw invalid_ui_args("Invalid number of arguments.");
    }

    messenger.clean_csv_files();
    return;
}
