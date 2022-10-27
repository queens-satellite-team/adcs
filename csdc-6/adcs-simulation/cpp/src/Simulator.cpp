/** @file Simulator.cpp
 *
 *@details class file that would configure and propagate the simulation
 *
 *@authors Lily de Loe, Justin Paoli
 *
 *Last Edited
 *2022-10-24
 *
**/
#include "Simulator.hpp"
#include "ConfigurationSingleton.cpp"
#include "SensorActuatorFactory.hpp"

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

    this->simulation_time = 0;
    this->timestep_length = timestep_length;

    this->satellite = new Satellite {
        Eigen::Vector3f(),
        Eigen::Vector3f(),
        Eigen::Vector3f(),
        Eigen::Matrix3f(),
    };
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

timestamp Simulator::update_simulation() {

}

timestamp Simulator::set_adcs_sleep(timestamp duration) {

}

timestamp Simulator::determine_time_passed() {

}

void Simulator::simulate(timestamp t) {
    
}

void Simulator::timestep() {
    // TODO: Check what type each actuator is and process accordingly
    Eigen::Vector3f total_rw_torques = Eigen::Vector3f::Zero();
    for (const auto &a : actuators) {
        Eigen::Vector3f omega_i = a.sim_get_current_velocities();
        Eigen::Vector3f alpha_i = a.sim_get_current_accelerations();
        Eigen::Matrix3f inertia_i = a.sim_get_inertia_matrix();
        total_rw_torques -= inertia_i * alpha_i;
        total_rw_torques -= satellite->omega_b.cross(inertia_i * omega_i);
    }

    Eigen::Matrix3f inertia_b_inverse = satellite->inertia_b.inverse();
    satellite->alpha_b = -inertia_b_inverse * 
        (satellite->omega_b.cross(satellite->inertia_b * satellite->omega_b));

    satellite->omega_b += satellite->alpha_b * (float) timestep_length;
    satellite->theta_b += satellite->omega_b * (float) timestep_length;
}

void Simulator::update_adcs_devices() {

}
