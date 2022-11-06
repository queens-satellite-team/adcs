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
 * 2022-10-11
 *
**/

#include <stdint.h>
#include <string>
#include <iomanip>

#pragma once
using namespace std;

/******************************************* DEFINITIONS *******************************************/

/* Compiler flags */

/* These flags define what version of the interface to use.
 *
 * DEF_INTERFACE -  default interface defined in this file. This is the base requirements for the
 *                  control code, all other interfaces must at least follow this convention.
 * SIM_INTERFACE -  interface used by the simulation.
 * STM_INTERFACE -  interface used by the STM32 driver.
 */
#define DEF_INTERFACE 0
#define SIM_INTERFACE 1
#define STM_INTERFACE 2

/* Flag to indicate which interface version to use */
#define INTERFACE SIM_INTERFACE

/********************************************* TYPES *********************************************/
/* Generic return type from the driver interface.
 *
 * @param sensor_return the measurement of the sensor
 * @param time_taken    time the value was returned
 *
 */
typedef struct driver_return
{
    measurement sensor_return;
    timestamp time_taken;
};

/* ADCS specific timestamp definition. All time measurements use this structure.
 *
 * @param milliseconds  number of milliseconds of the timestamp
 * @param seconds       number of seconds of the timestamp. This is rolled-over from milliseconds.
 *
 */
class timestamp
{
    public:
        /*
         * @name timestamp constructor
         *
         * @details sets initial values of millisecond and second fields.
         */
        timestamp(uint32_t milliseconds = 0, uint32_t seconds = 0) : millisecond(milliseconds), second(seconds) {}

        /*
         * @name milliseconds
         *
         * @details millisecond field accessor
         *
         * @returns millisecond field
         */
        uint32_t milliseconds() {return this->millisecond;}

        /*
         * @name seconds
         *
         * @details second field accessor
         *
         * @returns second field
         */
        uint32_t seconds() {return this->second;}

        /*
         * @name operator+ overload
         *
         * @details adds two timestamps. Overflow of milliseconds increments seconds. Overflow of
         *          seconds is explicitly allowed, the caller should deal with consequences of
         *          second overflow.
         *
         * @returns result of addition.
         */
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

        /*
         * @name operator- overload
         *
         * @details subtracts two timestamps. Underflow is explicitly allowed, millisecond
         *          underflow decrements the seconds field.
         *
         * @returns result of addition.
         */
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

        /*
         * @name operator+ overload
         *
         * @details adds two timestamps. Overflow of milliseconds increments seconds. Overflow of
         *          seconds is explicitly allowed, the caller should deal with consequences of
         *          second overflow.
         *
         * @returns result of addition.
         */
        timestamp operator+=(const timestamp& b)
        {
            return *this + b;
        }

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

        friend bool operator>(const timestamp& l, const timestamp& r)
        {
            return r < l;
        }

        friend bool operator<=(const timestamp& l, const timestamp& r)
        {
            return !(l > r);
        }

        friend bool operator>=(const timestamp& l, const timestamp& r)
        {
            return !(l < r);
        }

        friend bool operator==(const timestamp& l, const timestamp& r)
        {
            bool ret = false;
            if ((l.second      == r.second) &&
                (l.millisecond == r.millisecond))
            {
                true;
            }
            return ret;
        }

        friend bool operator!=(const timestamp& l, const timestamp& r)
        {
            return !(l==r);
        }

        string pretty_string()
        {
            uint32_t out_sec = (second + millisecond/1000);
            uint32_t out_mil = millisecond % 1000;
            uint32_t out_min = out_sec / 60;
            out_sec %= 60;

            stringstream formatter;
            formatter << "[" << out_min << ":" << setw(10) << setfill('0') << out_sec << ":";
            formatter << setw(10) << setfill('0') << out_mil << "]";

            return formatter.str();
        }

        /*
         * CAST FROM UINT32_T IN MS TO TIMESTAMP
         */

    private:
        uint32_t millisecond;
        uint32_t second;
};

/* Return structure from all sensors.
 *
 * @param vec           array with the values measured. Indices and frames must be maintained in
 *                      the implementation.
 * @param time_taken    time the measurement was taken.
 *
 */
typedef struct measurement
{
    vector<float> vec;
    timestamp       time_taken;
};

/* Actuator measurement structure. Used to quantify how much an actuator needs to change.
 *
 * @param required_values   the desired change in values of the actuator. Indices and frames must
 *                          be maintained in the implementation.
 * @param time_requested    time the actuator change was originally requested by the control code.
 *
 */
typedef struct actuator_state {
    float       acceleration;
    float       velocity;
    float       position;
    timestamp   time;
};

/******************************************* INTERFACES ******************************************/

#if DEF_INTERFACE == INTERFACE
/*
 * @class   Interface_Object
 *
 * @details This object is the highest level class in the interface. Each has a function that sends
 *          a command past the interface to the driver.
 *
 * @note    THIS IS AN ENTIRELY ABSTRACT CLASS.
 *
 */
class ADCS_device {
    public:
        /*
         * @name    time_until_ready
         *
         * @details this function determines how long until the device is ready to be polled.
         *
         * @returns 0 if the device is already in a good state, otherwise the amount of time until
         *          it is ready to be polled again.
         */
        virtual timestamp time_until_ready();

};

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
        virtual measurement take_measurement();

    private:
        /* latest measurement value taken */
        measurement current_value;
};

/*
 * @class Actuator
 *
 * @details this class defines a generic actuator object. It has functions to set a change in the
 *          actuator values, and to check when the actuator can be updated again.
 *
 * @implements ADCS_device
 *
 */
class Actuator : public ADCS_device {
    public:
        /*
         * @name            set_output
         *
         * @param new_value the new value to set the actuators to
         *
         */
        virtual void set_current_accelerations(action new_value); // TODO >> this may need a return value

};

/******************************************** CLASSES ********************************************/

/*
 * @class adcs_timer
 *
 * @details this class defines all timing interactions.
 *
 */
class adcs_timer {
    public:
        /*
         * @name get_time
         *
         * @returns the current time
         */
        timestamp   get_time();

        /*
         * @name sleep
         *
         * @details sets the control code to sleep for a requested amount of time.
         *
         * @param time the amount of time to sleep
         *
         */
        void sleep(timestamp time);
};

/*
 * @class accelerometer
 *
 * @details concrete Sensor implementation for accelerometers
 *
 * @implements ADCS_device, Sensor
 */
class accelerometer : public Sensor {};

/*
 * @class gyroscope
 *
 * @details concrete Sensor implementation for gyroscopes
 *
 * @implements ADCS_device, Sensor
 */
class gyroscope : public Sensor {};

/*
 * @class Reaction_wheel

 *
 * @details concrete Sensor implementation for reaction wheels
 *
 * @implements ADCS_device, Actuator
 */
class Reaction_wheel
 : public Actuator {};
#endif