/** @file sim_interface.hpp
 *
 * @details This file defines the interface between the control code and the simulation. It expands
 *          interface.hpp by adding simulation specific functions as required to the interface.
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-10-11
 *
**/

#include <def_interface.hpp>

#pragma once

#if SIM_INTERFACE == INTERFACE
/*
 * @class Sensor
 *
 * @details this class describes a generic sensor. It must have some way to request a measurement,
 *          and a way to ensure the sensor is ready for a new measurement.
 *
 * @implements ADCS_device
 *
 * @note    THIS IS AN ENTIRELY ABSTRACT CLASS.
 *
 */
class Sensor : public ADCS_device {
    public:
        /*
         * @name    take_measurement
         *
         * @details this function takes a measurement using the sensor.
         *
         * @returns the required measurement if succesful.
         *
         */
        virtual measurement take_measurement() = 0;

        /*
         * @name    sim_set_current_vals
         *
         * @details this function sets the new raw sensor values from the simulation and 
         */
        virtual void sim_set_current_vals(float** new_vals, uint32_t axes, uint32_t num_sensors);

    private:
        /* Latest measurement value taken */
        measurement current_vector_value;

        /* Number of physical sensors in the sensor unit*/
        uint32_t num_sensors;

        /* Number of axes per physical sensor */
        uint32_t num_axes;
};

/*
 * @class Actuator
 *
 * @details this class defines a generic actuator object. It has functions to set a change in the
 *          actuator values, and to check when the actuator can be updated again.
 *
 * @param
 *
 * @implements ADCS_device
 *
 */
class Actuator : public ADCS_device {
    public:

        /*
         * @name Actuator constructor
         *
         * @details constructor for actuators. All initial values for the actuators are 0, other
         *          than the number of actuators, which is required.
         */
        Actuator(uint32_t num_act);

        /*
         * @name    set_output
         *
         * @param   new_value the new value to set the actuators to. This is called by the control
         *          code. The size should be the number of actuators.
         *
         */
        virtual void set_output(); // TODO >> this may need a return value

        /*
         * @name    sim_get_current_accelerations
         *
         * @details accessor for the simulation to get the current accelerations of the actuator.
         *
         * @param current_accelerations
         *          an empty array of actions to be filled by the function. If succesful, this
         *          function fills this array with the current accelerations of each actuator.
         *
         * @param num_actuators
         *          the number of actuators in (ie size of) the current_acclerations array.
         */
        virtual Eigen::Vector3f sim_get_current_accelerations();

        /*
         * @name    sim_get_current_velocities
         *
         * @details accessor for the simulation to get the current velocities of the actuator.
         *
         * @param current_accelerations
         *          an empty array of actions to be filled by the function. If succesful, this
         *          function fills this array with the current accelerations of each actuator.
         *
         * @param num_actuators
         *          the number of actuators in (ie size of) the current_velocities array.
         */
        virtual Eigen::Vector3f sim_get_current_velocities();

        virtual Eigen::Matrix3f sim_get_inertia_matrix();

    private:
        /* Number of physical actuators in the sensor unit*/
        uint32_t num_actuators;

        /* Current acceleration values for each actuator*/
        Eigen::Matrix<action, Eigen::Dynamic, 1> current_accelerations;

        /* Current velocity values for each actuator */
        Eigen::Matrix<action, Eigen::Dynamic, 1> current_velocities;

};

/******************************************** CLASSES ********************************************/

/*
 * @class adcs_timer
 *
 * @details this class defines all timing interactions.
 *
 */
class ADCS_timer {
    public:
        /*
         * @name get_time
         *
         * @returns the current time
         */
        timestamp get_time();

        /*
         * @name sleep
         *
         * @details sets the control code to sleep for a requested amount of time.
         *
         * @param time the amount of time to sleep
         *
         */
        void sleep(timestamp time);

        void sim_update_cur_time(timestamp time);
};

/*
 * @class accelerometer
 *
 * @details concrete Sensor implementation for accelerometers
 *
 * @implements ADCS_device, Sensor
 */
class Accelerometer : public Sensor {};

/*
 * @class gyroscope
 *
 * @details concrete Sensor implementation for gyroscopes
 *
 * @implements ADCS_device, Sensor
 */
class Gyroscope : public Sensor {};

/*
 * @class reaction_wheel
 *
 * @details concrete Sensor implementation for reaction wheels
 *
 * @implements ADCS_device, Actuator
 */
class Reaction_wheel : public Actuator {
    public:
        Reaction_wheel(uint32_t num_wheels) : Actuator(num_wheels) {};
};
#endif
