/** @file Simulator.hpp
 *
 *@details header file for class file that would configure and propagate the simulation
 *
 *@authors Lily de Loe
 *
 *Last Edited
 *2022-10-24
 *
**/

#ifndef SIMULATOR_HPP_
#define SIMULATOR_HPP_

#include "sim_interface.hpp"
#include <memory>
#include <vector>

class Simulator {
public:
    Simulator(const std::string &configFile);

private:
    //test using a dummy function in the control code  
    void step(); //advances each sensor -> remove this because the control code will update
    void create_actuator(const std::string &name, const std::string &type);
    void create_sensor(const std::string &name, const std::string &type);
    void set_command(std::string name, double command);
    double get_sensor_value(std::string name);
    void Simulator::print_stats(); //use to read the simulator data

private:
    //unorder_maps relate strings to the names of objects
    std::unordered_map<std::string, std::unique_ptr<Sensor>> sensors;
    std::unordered_map<std::string, std::unique_ptr<Actuator>> actuators;

    double timestep;
    SatelliteState state;
    double currentTime;
    bool doStats; //use for printstats()
};

#endif
