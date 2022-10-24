/** @file Simulator.cpp
 *
 *@details class file that would configure and propagate the simulation
 *
 *@authors Lily de Loe
 *
 *Last Edited
 *2022-10-24
 *
**/
#include "Simulator.hpp"
#include "ConfigurationSingleton.cpp"
#include "simulator/SensorActuatorFactory.hpp"


using namespace std::chrono_literals;
Simulator::Simulator(const std::string &configFile):doStats(true) {

    auto &config = Configuration::GetInstance();
    if (!config.Load(configFile)) {
        std::cout <<"Configuration Failed to load"<< std::endl;
    }

    for (const auto &sensor : config.GetSensorConfigs()) {
        //first is string, second is data (from map)
        create_sensor(sensor.first);
    }

    for (const auto &actuator : config.GetActuatorConfigs()) {
        //first is string, second is data (from map)
        create_actuator(sensor.first);
    }

    //these would be used if GetTimeStep and PrintStats() are used
    //timestep = config.GetTimeStep();
    doStats = config.IsPrintStats();
}

//create sensor based on name
void Simulator::create_sensor(const std::string &name) {
    if (name.empty()) {
        std::cout <<"Device name must be populated. Got "
        << name << std::endl;
        return;
    }

    auto sensorPtr = SensorActuatorFactory::GetSensor(name);
    if (!sensorPtr) {
        std::cout << "Unknown sensor type: " << name << std::endl;
        return;
    }

    sensors[name] = std::move(sensorPtr);
}

//create actuator based on name
void Simulator::create_actuator(const std::string &name) {
    if (name.empty()) {
        std::cout << "Device name must be populated. Got "
        << name << std::endl;
        return;
    }

    auto actPtr = SensorActuatorFactory::GetActuator(name);
    if (!actPtr) {
        std::cout << "Unknown actuator type: " << name << std::endl;
        return;
    }

    actuators[name] = std::move(actPtr);
}

void Simulator::set_command(std::string name, double command)
{
    //need a check that the actuator name is in the map
    actuators[name]->SetCommandValue(command);
}

double Simulator::get_sensor_value(std::string name)
{
    //need a check that the sensor name is in the map
    return sensors[name]->GetValue();
}

//will be necessary if time step functionality is added to this class
/*
void Simulator::step() {
    //propagate sensors forward in time
    //in series instead of parallel (single threaded)
    for (auto &sens : sensors) {
        //tick would belong to adcs device
        sens->Tick(timestep, state);
    }

    //propagate actuators forward in time
    for (auto &act : actuators) {
        //tick would belong to adcs device
        act->Tick(timestep, state);
    }

    for (const auto &act : actuators) {
        //GetChangeInState() would belong to actuator
        state += act.second->GetChangeInState(); //change in ang vel, accel, pos, from each actuator
    }

    //simulation would be called here
    //call_simulation();

    //this is where print_stats() would be called in this implementation
    if (doStats) {
        print_stats();
    }
  
    //incrament time
    currentTime += timestep;
}
*/

//propogate the math forwards in time 
//this is where simulation the math should go in my implementation
/*
void Simulator::call_simulation() {}
*/

//print statistics on code performance
//optional, but will be good for debugging
//in my implementation, this is called in step()
void Simulator::print_stats() {
  std::cout << "NAME: \t\tTIME" << std::endl;
  for (const auto &act : actuators) {
    std::cout << act.first<< ": \t\t"
        << act.second->GetLastTickDuration() << std::endl;
  }

  for (const auto &sens : sensors) {
    std::cout << sens.first << ": \t\t"
        << sens.second->GetLastTickDuration() << std::endl;
  }
}
