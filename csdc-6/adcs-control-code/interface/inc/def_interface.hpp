/** @file def_interface.hpp
 *
 * @details This file defines the default interface between the control code and the driver. It has
 *          class deffinitions for actuators, sensors, and a timer.
 *
 *          This interface is intended as the required interaface definitions that the control code
 *          needs, and hence all specific interfaces must include the same structure at a minimum.
 *
 * @authors Aidan Sheedy
 *
 * Last Edited
 * 2022-11-07
 *
**/

#include <stdint.h>
#include <string>
#include <Eigen/Dense>
#include <iomanip>

#pragma once

/******************************************* DEFINITIONS *******************************************/

/* Compiler flags**/

/* These flags define what version of the interface to use.
 *
 * DEF_INTERFACE -  default interface defined in this file. This is the base requirements for the
 *                  control code, all other interfaces must at least follow this convention.
 * SIM_INTERFACE -  interface used by the simulation.
 * STM_INTERFACE -  interface used by the STM32 driver.
**/
#define DEF_INTERFACE 0
#define SIM_INTERFACE 1
#define STM_INTERFACE 2

/* Flag to indicate which interface version to use**/
#define INTERFACE SIM_INTERFACE

/********************************************* TYPES *********************************************/
/* ADCS specific timestamp definition. All time measurements use this structure.
 *
 * @param milliseconds  number of milliseconds of the timestamp
 * @param seconds       number of seconds of the timestamp. This is rolled-over from milliseconds.
 *
**/
class timestamp
{
    public:
        /**
         * @name timestamp constructor
         *
         * @details sets initial values of millisecond and second fields.
        **/
        timestamp(uint32_t milliseconds = 0, uint32_t seconds = 0) : millisecond(milliseconds), second(seconds) {}

        explicit timestamp(float real_person_time) : millisecond((float)(real_person_time - ((int)real_person_time)) * 1000.0), second(real_person_time){}; // check this conversion later its a bit hacky
        /**
         * @name milliseconds
         *
         * @details millisecond field accessor
         *
         * @returns millisecond field
        **/
        uint32_t milliseconds() {return this->millisecond;}

        /**
         * @name seconds
         *
         * @details second field accessor
         *
         * @returns second field
        **/
        uint32_t seconds() {return this->second;}

        /**
         * @name operator+ overload
         *
         * @details adds two timestamps. Overflow of milliseconds increments seconds. Overflow of
         *          seconds is explicitly allowed, the caller should deal with consequences of
         *          second overflow.
         *
         * @returns result of addition.
        **/
        timestamp operator+(const timestamp& b)
        {
            timestamp result;

            if (b.second > UINT32_MAX - this->second)
            {
                // explicitly allow overflow of the seconds field
                result.second = (b.second > this->second) ? (b.second - this->second) : (this->second - b.second);
            }
            else
            {
                result.second = b.second + this->second;
            }

            if (b.millisecond > UINT32_MAX - this->millisecond)
            {
                // overflow milliseconds and increment seconds by 1. Again, allow seconds to overflow.
                result.millisecond = (b.millisecond > this->millisecond) ? (b.millisecond - this->millisecond) : (this->millisecond - b.millisecond);
                result.second = (UINT32_MAX == result.second) ? 0 : result.second + 1;
            }
            else
            {
                result.millisecond = this->millisecond + b.millisecond;
            }

            return result;
        }

        /**
         * @name operator- overload
         *
         * @details subtracts two timestamps. Underflow is explicitly allowed, millisecond
         *          underflow decrements the seconds field.
         *
         * @returns result of addition.
        **/
        timestamp operator-(const timestamp& b)
        {
            timestamp result;

            if (b.second > this->second)
            {
                // explicitly allow underflow of the seconds field
                result.second = UINT32_MAX - (b.second - this->second);
            }
            else
            {
                result.second = this->second - b.second;
            }

            if (b.millisecond > this->millisecond)
            {
                // underflow milliseconds and decrement seconds by 1. Again, allow seconds to underflow.
                result.millisecond = UINT32_MAX - (b.millisecond - this->millisecond);
                result.second = (0 == result.second) ? UINT32_MAX : result.second - 1;
            }
            else
            {
                result.millisecond = this->millisecond - b.millisecond;
            }

            return result;
        }

        explicit operator float() const { return this->millisecond / 1000.0 + (float) this->second; }

        /**
         * @name operator+ overload
         *
         * @details adds two timestamps. Overflow of milliseconds increments seconds. Overflow of
         *          seconds is explicitly allowed, the caller should deal with consequences of
         *          second overflow.
         *
         * @returns result of addition.
        **/
        timestamp operator+=(const timestamp& b)
        {
            return *this + b;
        }

        /**
         * @name operator< overload
         *
         * @details Compares two timestamps.
         *
         * @returns true if the left side is smaller than the right side. If seconds are equal, it
         *               checks milliseconds.
         *          false if the left side is not smaller than the right.
        **/
        friend bool operator<(const timestamp& l, const timestamp& r)
        {
            bool ret = false;
            
            if ( (l.second      < r.second)      ||
               ( (l.millisecond < r.millisecond) &&
                 (l.second      <= r.second)     ))
            {
                ret = true;
            }

            return ret;
        }

        /**
         * @name operator> overload
         *
         * @details compares two timestamps.
         *
         * @returns true if the right side is smaller than the left side. If seconds are equal, it
         *               checks milliseconds.
         *          false if the right side is not smaller than the left.
        **/
        friend bool operator>(const timestamp& l, const timestamp& r)
        {
            return r < l;
        }

        /**
         * @name operator<= overload
         *
         * @details Compares two timestamps.
         *
         * @returns true if the left side is smaller or equal to the right side. 
         *          false otherwise.
        **/
        friend bool operator<=(const timestamp& l, const timestamp& r)
        {
            return !(l > r);
        }

        /**
         * @name operator>= overload
         *
         * @details Compares two timestamps.
         *
         * @returns true if the right side is smaller or equal to the left side. 
         *          false otherwise.
        **/
        friend bool operator>=(const timestamp& l, const timestamp& r)
        {
            return !(l < r);
        }

        /**
         * @name operator== overload
         *
         * @details Compares two timestamps.
         *
         * @returns true if the both sides are equal.
         *          false otherwise.
        **/
        friend bool operator==(const timestamp& l, const timestamp& r)
        {
            bool ret = false;
            if ((l.second      == r.second) &&
                (l.millisecond == r.millisecond))
            {
                ret = true;
            }
            return ret;
        }

        /**
         * @name operator== overload
         *
         * @details Compares two timestamps.
         *
         * @returns true if the both sides are not equal.
         *          false otherwise.
        **/
        friend bool operator!=(const timestamp& l, const timestamp& r)
        {
            return !(l==r);
        }

        /**
         * @name    pretty_string
         *
         * @details converts the timestamp to an easily readable string.
         *
         * @returns a string of the format:
         *          [mm:ss:msms]
        **/
        std::string pretty_string()
        {
            uint32_t out_sec = (second + millisecond/1000);
            uint32_t out_mil = millisecond % 1000;
            uint32_t out_min = out_sec / 60;
            out_sec %= 60;

            std::stringstream formatter;
            formatter << "[" << out_min << ":" << std::setw(2) << std::setfill('0') << out_sec << ":";
            formatter << std::setw(4) << std::setfill('0') << out_mil << "]";

            return formatter.str();
        }

    private:
        /* the milliseconds of the timestamp. If overflowed, second is incremented. */
        uint32_t millisecond;

        /* seconds of the timestamp. */
        uint32_t second;
};

/**
 * @struct  measurement
 * 
 * @details Return structure from all sensors.
 *
 * @param vec           array with the values measured. Indices and frames must be maintained in
 *                      the implementation.
 * @param time_taken    time the measurement was taken.
 *
**/
typedef struct
{
    Eigen::Vector3f vec;
    timestamp       time_taken;
} measurement;

typedef struct
{
    Eigen::Vector3f position;
    Eigen::Vector3f velocity;
    Eigen::Vector3f acceleration;
    timestamp       time_taken;
} gyro_state;

/**
 * @struct  actuator_state
 * 
 * @details Actuator measurement structure. Used to quantify how much an actuator needs to change.
 *
 * @param required_values   the desired change in values of the actuator. Indices and frames must
 *                          be maintained in the implementation.
 * @param time_requested    time the actuator change was originally requested by the control code.
 *
**/
typedef struct {
    float       acceleration;
    float       velocity;
    float       position;
    timestamp   time;
} actuator_state;

/******************************************* INTERFACES ******************************************/

#if DEF_INTERFACE == INTERFACE
/**
 * @class   Interface_Object
 *
 * @details This object is the highest level class in the interface. Each has a function that sends
 *          a command past the interface to the driver.
 *
 * @note    THIS IS AN ENTIRELY ABSTRACT CLASS.
 *
**/
class ADCS_device {
    public:
        /**
         * @name    time_until_ready
         *
         * @details this function determines how long until the device is ready to be polled.
         *
         * @returns 0 if the device is already in a good state, otherwise the amount of time until
         *          it is ready to be polled again.
        **/
        virtual timestamp time_until_ready();

};

/**
 * @class Sensor
 *
 * @details this class describes a generic sensor. It must have some way to request a measurement,
 *          and a way to ensure the sensor is ready for a new measurement.
 *
 * @implements ADCS_device
 *
 * @note    THIS IS AN ENTIRELY ABSTRACT CLASS.
 *
**/
class Sensor : public ADCS_device {
    public:
        /**
         * @name    take_measurement
         *
         * @details this function takes a measurement using the sensor.
         *
         * @returns the required measurement if succesful.
         *
        **/
        virtual measurement take_measurement();

        /**
         * @name    get_positions
         *
         * @details positions vector accessor.
         *
         * @returns positions of each physical sensor.
        **/
        std::vector<Eigen::Vector3f> get_positions();

    private:
        /* latest measurement value taken**/
        measurement current_value;

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
 *
**/
class Actuator : public ADCS_device {
    public:
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
        virtual void set_target_state(actuator_state target_state);

        /**
         * @name    get_target_state
         *
         * @details accessor for the target state of the actuator.
         *
         * @returns the current target state of the actuator.
        **/
        actuator_state get_target_state();

        /**
         * @name    get_position
         *
         * @details accessor for the position of the actuator.
         *
         * @returns the position within the satellite of the actuator.
        **/
        Eigen::Vector3f get_position();
};

/******************************************** CLASSES ********************************************/

/**
 * @class adcs_timer
 *
 * @details this class defines all timing interactions.
 *
**/
class adcs_timer {
    public:
        /**
         * @name get_time
         *
         * @returns the current time
        **/
        timestamp   get_time();

        /**
         * @name sleep
         *
         * @details sets the control code to sleep for a requested amount of time.
         *
         * @param time the amount of time to sleep
         *
        **/
        void sleep(timestamp time);
};

/**
 * @class accelerometer
 *
 * @details concrete Sensor implementation for accelerometers
 *
 * @implements ADCS_device, Sensor
**/
class accelerometer : public Sensor 
{
    public:
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
class gyroscope : public Sensor
{
    public:
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
 * @class Reaction_wheel

 *
 * @details concrete Sensor implementation for reaction wheels
 *
 * @implements ADCS_device, Actuator
**/
class Reaction_wheel : public Actuator
 {
    public:
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
 };
#endif
