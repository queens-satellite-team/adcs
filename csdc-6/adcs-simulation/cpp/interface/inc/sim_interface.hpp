/** @file   sim_interface.hpp
 *
 * @details This file defines the interface between the control code and the simulation. It expands
 *          interface.hpp by adding simulation specific functions as required to the interface.
 *
 * @authors Aidan Sheedy, Lily de Loe
 *
 * Last Edited
 * 2022-11-08
 *
**/
#include <vector>
#include <Eigen/Dense>

#include "def_interface.hpp"
#include "Simulator.hpp"

#pragma once

#if SIM_INTERFACE == INTERFACE

/**
 * @class   Interface_Object
 *
 * @details This object is the highest level class in the interface, and describes any physical
 *          in the ADCS. Primary devices are Actuators and Sensors, and they all have some polling
 *          rate that is implemented in this class.
 *
**/
class ADCS_device {
    public:
        /**
         * @name ADCS_device constructor
         *
         * @details constructor for ADCS_device.
         * 
         * @param polling_time the minimum amount of time between polling events for the device
         * @param sim          simulator object used to update and get updates about the device
        **/
        ADCS_device(timestamp polling_time, Simulator* sim);

        /**
         * @name ADCS_device destructor
         *
         * @details virtual as this class is a base class with virtual functions.
        **/
        virtual ~ADCS_device(){}

        /**
         * @name    time_until_ready
         *
         * @details this function determines how long until the device is ready to be polled.
         *
         * @returns 0 if the device is already in a good state, otherwise the amount of time until
         *          it is ready to be polled again.
        **/
        virtual timestamp time_until_ready();

    private:
        // TODO: This may need an accessor - for now not implementing.
        /* Minimum amount of time that must pass between each time the device is polled.**/
        timestamp min_polling_increment;

        /* Last time the device was polled.**/
        timestamp last_polled;

    protected:
        /**
         * @name    update_poll_time
         *
         * @details this function updates the last time the last time the device was polled.
         *
         * @param   new_time the latest poll time of the device.
        **/
        void update_poll_time(timestamp new_time);

        /**
         * @name sim
         *
         * @details Pointer to the simulation object that instantiated the ADCS_device. Used to
         *          tell the simulation to advance. This essentially "passes control" back to the
         *          simulation.
        **/
        Simulator* sim;

};

/**
 * @class Sensor
 *
 * @details this class describes a generic sensor. It must have some way to request a measurement,
 *          and a way to ensure the sensor is ready for a new measurement.
 *
 * @implements ADCS_device
**/
class Sensor : public ADCS_device {
    public:
        /**
         * @name Sensor constructor
         *
         * @details constructor for Sensor. Only check is to see if the sim is void. Passes
         *          polling_time and sim to parent class, and populates all other parameters if
         *          they are valid.
         * 
         * @param polling_time  polling time of the sensor
         * @param sim           simulator object used to update and get updates about the device
         * @param positions     positions of all sensor locations on the satellite
         * @param num_sensors   number of sensor locations on the satellite
         * @param num_axes      number of axes each sensor location measures
        **/
        Sensor(timestamp polling_time, Simulator* sim, std::vector<Eigen::Vector3f> positions, uint32_t num_sensors, uint32_t num_axes);

        /**
         * @name Sensor destructor
         *
         * @details virtual as this class is a base class with virtual functions.
        **/
        virtual ~Sensor(){}

        /**
         * @name    set_current_vals
         *
         * @details this function sets the new raw sensor values from the simulation and calculates
         *          what the sensor measurement vector is from the physical values. The default
         *          implementation assumes that num_axes is 3, and num_sensors is one, and that no
         *          computation needs to be made.
         *
         * @param   physical_vals   vector of physical measuremnts for each sensor. The vector size
         *                          is the number of sensors, and the matrix size is the number of
         *                          axis for each measurement.
         * @param   time            the time the measurement was taken.
        **/
        virtual void set_current_vals(std::vector<Eigen::VectorXf> physical_vals, timestamp time);

        /**
         * @name    get_positions
         *
         * @details positions vector accessor.
         *
         * @returns positions of each physical sensor.
        **/
        std::vector<Eigen::Vector3f> get_positions();

    protected:
        /* Latest measurement value taken**/
        measurement current_vector_value;

    private:
        /* Number of physical sensors in the sensor unit*/
        uint32_t num_sensors;

        /* Number of axes per physical sensor**/
        uint32_t num_axes;

        /* Positions of each satellite within the satellite body. Dimensions are num_sensors x 3**/
        std::vector<Eigen::Vector3f> positions;
};

/**
 * @class Actuator
 *
 * @details this class defines a generic actuator object. It has functions to set a change in the
 *          actuator values, and to check when the actuator can be updated again.
 *
 * @implements ADCS_device
**/
class Actuator : public ADCS_device
{
    public:
        /**
         * @name Actuator constructor
         *
         * @details constructor for actuators. Initial values for current and target states are 0.
        **/
        Actuator(timestamp polling_time, Simulator* sim, Eigen::Vector3f position, actuator_state max_vals, actuator_state min_vals, actuator_state initial_vals, Eigen::Vector3f axis_of_rotation);

        /**
         * @name Actuator destructor
         *
         * @details virtual as this class is a base class with virtual functions.
        **/
        virtual ~Actuator(){}

        /**
         * @name    get_current_state
         *
         * @details accessor for the current state of the actuator. This includes current
         *          accelerations, velocities, and positions (if applicable).
         *
         * @returns the current state of the actuator
        **/
        virtual actuator_state get_current_state() // May need to add a default implementation
        {
            actuator_state empty;
            return empty;
        }

        /**
         * @name    get_target_state
         *
         * @details accessor for the target state of the actuator.
         *
         * @returns the current target state of the actuator.
        **/
        actuator_state get_target_state();

        /**
         * @name    set_target_state
         *
         * @details sets the target state the actuator is requesting from the simulation.
         *          Interpretation of the target is actuator-dependedent. Updates the simulation
         *          when called.
         *
         * @param   target_state the new state the control code would like to be in.
         *
        **/
        virtual void set_target_state(actuator_state target_state)
        {
            this->target_state = target_state;
            return;
        } // MAY NEED TO REMOVE THIS OR CHANGE IT

        /**
         * @name    get_position
         *
         * @details accessor for the position of the actuator.
         *
         * @returns the position within the satellite of the actuator.
        **/
        Eigen::Vector3f get_position();

        /**
         * @name    get_axis_of_rotation
         *
         * @details accessor for the axis of rotation of the actuator.
         *
         * @returns the axis of rotation of the actuator.
        **/
        Eigen::Vector3f get_axis_of_rotation();

        /**
         * @name get_max_acceleration
         * @returns [float], the maximum acceleration of the actuator
         * 
         * @details accessor for the max acceleration of the actuator
        **/
       float get_max_acceleration();

    protected:
        /**
         * @name    set_current_state
         *
         * @details sets the output values (acceleration, velocities, and position as applicable)
         *          of the actuator.
         *
         * @param   new_state the new state of the actuator. This is set by the simulation.
        **/
        void set_current_state(actuator_state new_state);

        /**
         * @name    check_valid_state
         * 
         * @details checks if the provided state is valid. 
         * 
         * @param state the state to check
         * 
         * @throws  ______ exception if the state is not valid. If no exception is thrown, it is 
         *          valid.
        **/
        void check_valid_state(actuator_state state);

        // is this ever used?
        /* Target state of the actuator. */
        actuator_state target_state;

        /* The position in the satellite of the actuator.**/
        Eigen::Vector3f position;

        /* Current acceleration values for each actuator*/
        actuator_state current_state;

        Eigen::Vector3f axis_of_rotation;
    private:
        /* The maximum value for each state property of the actuator.**/
        actuator_state max_state_values;

        /* The minimum value for each state property of the actuator.**/
        actuator_state min_state_values;

};

/**************************************** CONCRETE CLASSES ***************************************/

/**
 * @class adcs_timer
 *
 * @details this class defines all timing interactions.
 *
**/
class ADCS_timer
{
    public:
        /**
         * @name ADCS_timer constructor
         *
         * @details constructor for the ADCS_timer. Uses the simulator to get the actual time.
        **/
        ADCS_timer(Simulator* sim);

        /**
         * @name get_time
         *
         * @returns the current time
        **/
        timestamp get_time();

        /**
         * @name sleep
         *
         * @details sets the control code to sleep for a requested amount of time.
         *
         * @param time the amount of time to sleep
         *
        **/
        timestamp sleep(timestamp duration);

    private:
        /* Pointer to the simulation object, used to get the current time.**/
        Simulator* sim;
};

/**
 * @class accelerometer
 *
 * @details concrete Sensor implementation for accelerometers
 *
 * @implements ADCS_device, Sensor
**/
class Accelerometer : public Sensor {
    public:
        /**
         * @name Accelerometer constructor
         * 
         * @details all values are directly passed to the Sensor base class. Number of sensors is
         *          one, and the number of axes is 3.
         * 
         * @param polling_time  polling time of the sensor
         * @param sim           simulator object used to update and get updates about the device
         * @param positions     positions of all sensor locations on the satellite
        */
        Accelerometer(timestamp polling_time, Simulator* sim, Eigen::Vector3f positions) : Sensor(polling_time, sim, {positions}, 1, 3) {}

        /**
         * @name    take_measurement
         *
         * @details this function takes a measurement using the sensor. The simulation is also told
         *          to update.
         *
         * @returns the required measurement if succesful.
        **/
        measurement take_measurement();
};

/**
 * @class gyroscope
 *
 * @details concrete Sensor implementation for gyroscopes
 *
 * @implements ADCS_device, Sensor
**/
class Gyroscope : public Sensor {
    public:
        /**
         * @name Gyroscope constructor
         * 
         * @details all values are directly passed to the Sensor base class. Number of sensors is
         *          one, and the number of axes is 3.
         * 
         * @param polling_time  polling time of the sensor
         * @param sim           simulator object used to update and get updates about the device
         * @param positions     positions of all sensor locations on the satellite
        */
        Gyroscope(timestamp polling_time, Simulator* sim, Eigen::Vector3f positions) : Sensor(polling_time, sim, {positions}, 1, 3) {}

        /**
         * @name    take_measurement
         *
         * @details this function takes a measurement using the sensor. The simulation is also told
         *          to update.
         *
         * @returns the required measurement if succesful.
        **/
        gyro_state take_measurement();
};

/**
 * @class Reaction_wheel
 *
 * @details concrete Actuator implementation for reaction wheels
 *
 * @implements ADCS_device, Actuator
**/
class Reaction_wheel : public Actuator
{
    public:
        /**
         * @name Reaction_wheel constructor
         *
         * @details constructor for the Reaction_wheel. All parameters are passed to the Actuator
         *          base class.
        **/
        Reaction_wheel(timestamp polling_time, Simulator* sim, Eigen::Vector3f position, actuator_state max_vals, actuator_state min_vals, actuator_state initial_vals, Eigen::Vector3f axis_of_rotation, float inertia_matrix);

        /**
         * @name    get_inertia_matrix
         *
         * @details inertia matrix accessor
         *
         * @returns the inertia matrix of a reaction wheel
         *
        **/
        float get_inertia_matrix();

        /**
         * @name    set_target_state
         *
         * @details sets the target state the actuator is requesting from the simulation.
         *          Interpretation of the target is actuator-dependedent. Updates the simulation
         *          when called.
         *
         * @param   target_state the new state the control code would like to be in.
         *
        **/
        void set_target_state(actuator_state target_state);

        /**
         * @name    get_current_state
         *
         * @details accessor for the current state of the actuator. This includes current
         *          accelerations, velocities, and positions (if applicable).
         *
         * @returns the current state of the actuator
        **/
        actuator_state get_current_state();

    private:

        /**
         * @name    inertia_matrix
         *
         * @details inertia matrix for a reaction wheel. It is assumed that all reaction wheels
         *          have the same inertia matrix in their frame of reference.
        **/
        float inertia_matrix;
};

#endif
