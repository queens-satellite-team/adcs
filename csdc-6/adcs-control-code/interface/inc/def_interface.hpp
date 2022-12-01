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
 * 2022-11-18
 *
**/

#include <stdint.h>
#include <string>
#include <Eigen/Dense>
#include <iomanip>

#include "adcs_exception.hpp"

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
/**
 * @exception invalid_ui_args
 *
 * @details exception used to indicate that a UI function has received bad arguments from the user.
**/
class timestamp_nullptr : public adcs_exception
{
    public:
        timestamp_nullptr(const char* msg) : adcs_exception(msg) {}
};

/** ADCS specific timestamp definition. All time measurements use this structure.
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
        timestamp(uint32_t milliseconds = 0, uint32_t seconds = 0) : millisecond(milliseconds), second(seconds)
        {
            rebalance_timestamp(&this->millisecond, &this->second);
        }

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

            uint32_t l_millisecond = this->millisecond;
            uint32_t r_millisecond = b.millisecond;
            uint32_t l_second = this->second;
            uint32_t r_second = b.second;

            if (r_second > UINT32_MAX - l_second)
            {
                // explicitly allow overflow of the seconds field
                result.second = (r_second > l_second) ? (r_second - l_second) : (l_second - r_second);
            }
            else
            {
                result.second = r_second + l_second;
            }

            uint32_t added_mil = 0;
            uint32_t added_sec = 0;
            add_milliseconds(l_millisecond, r_millisecond, &added_mil, &added_sec);

            result.millisecond = added_mil;
            result.second += added_sec;
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

            rebalance_timestamp(&result.millisecond, &result.second);
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
        /**
         * @name    rebalance_timestamp
         *
         * @details given milliseconds and seconds fields, restructures the time so that the
         *          maximum amount of time is in milliseconds as possible.
         *
         * @param p_milliseconds the milliseconds field to rebalance.
         * @param p_seconds      the seconds field to rebalance.
        **/
        void rebalance_timestamp(uint32_t *p_milliseconds, uint32_t * p_seconds)
        {
            if ((nullptr == p_milliseconds) ||
                (nullptr == p_seconds) )
            {
                throw timestamp_nullptr("Pointer in \"rebalance_timestamp\" is null.");
            }

            uint32_t in_seconds      = *p_seconds;
            uint32_t in_milliseconds = *p_milliseconds;

            uint32_t out_seconds = 0;
            uint32_t out_milliseconds = 0;

            uint32_t seconds_to_milliseconds = 0;
            uint32_t add_out_mil = 0;
            uint32_t add_out_sec = 0;

            // timestamps always use milliseconds first - if seconds is populated, try and move it to ms.
            if ((UINT32_MAX / 1000) > in_seconds)
            {
                seconds_to_milliseconds = in_seconds * 1000;
                add_milliseconds(seconds_to_milliseconds, in_milliseconds, &add_out_mil, &add_out_sec); 
            }
            else
            {
                uint32_t saturation_milliseconds = UINT32_MAX - 1000;
                out_seconds = in_seconds - saturation_milliseconds / 1000;
                add_milliseconds(saturation_milliseconds, in_milliseconds, &add_out_mil, &add_out_sec);
            }

            out_seconds += add_out_sec;
            out_milliseconds = add_out_mil;

            *p_seconds = out_seconds;
            *p_milliseconds = out_milliseconds;
        }

        /**
         * @name    add_milliseconds
         *
         * @details adds two millisecond fields such that overflow populates a seconds field. The
         *          algorithm is such that milliseconds are always favoured, and seconds are only
         *          populated once milliseconds are full. Upon overflow, the milliseconds field
         *          will loose 1000 milliseconds and be converted to seconds.
         *
         * @param p_milliseconds the milliseconds field to rebalance.
         * @param p_seconds      the seconds field to rebalance.
        **/
        void add_milliseconds(uint32_t a, uint32_t b, uint32_t *p_out_mil, uint32_t *p_out_sec)
        {
            if ((nullptr == p_out_mil) ||
                (nullptr == p_out_sec) )
            {
                throw timestamp_nullptr("Pointer in \"add_milliseconds\" is null.");
            }

            uint32_t out_sec = 0;
            uint32_t out_mil = 0;
            uint32_t r_millisecond = b;
            uint32_t l_millisecond = a;
            // This might not have to be a while loop, but it works for now.
            while (r_millisecond > UINT32_MAX - l_millisecond)
            {
                out_sec++;
                if (r_millisecond > l_millisecond)
                {
                    r_millisecond -= 1000;
                }
                else
                {
                    l_millisecond -= 1000;
                }
            }

            out_mil = r_millisecond + l_millisecond;

            *p_out_mil = out_mil;
            *p_out_sec = out_sec;
            return;
        }

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

/******************************************* EXCEPTIONS ******************************************/
/**
 * @exception device_not_ready
 *
 * @details exception used to indicate that an adcs device has not completed it's required poll delay.
**/
class device_not_ready : public adcs_exception
{
    public:
        device_not_ready(const char* msg) :  adcs_exception(msg) {}
};

/**
 * @exception invalid_actuator_state
 *
 * @details exception used to indicate that an actuator is in an invalid state.
**/
class invalid_actuator_state : public adcs_exception
{
    public:
        invalid_actuator_state(const char* msg) :  adcs_exception(msg) {}
};

/**
 * @exception invalid_dimensions_error
 *
 * @details exception used to indicate that invalid dimensions were provided.
**/
class invalid_dimensions_error : public adcs_exception
{
    public:
        invalid_dimensions_error(const char* msg) :  adcs_exception(msg) {}
};

/**
 * @exception invalid_adcs_param
 *
 * @details exception used to indicate that an adcs device has not been provided a valid parameter.
**/
class invalid_adcs_param : public adcs_exception
{
    public:
        invalid_adcs_param(const char* msg) :  adcs_exception(msg) {}
};

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
