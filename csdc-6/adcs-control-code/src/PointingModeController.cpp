/**
 * @file PointingModeController.cpp
 *
 * @details Implementation for the pointing mode controller
 *
 * @authors Justin Paoli
 *
 * Last Edited
 * 2022-11-13
**/

#include "PointingModeController.hpp"

PointingModeController::PointingModeController(
    std::unordered_map<std::string, std::shared_ptr<Sensor>> sensors,
    std::unordered_map<std::string, std::shared_ptr<Actuator>> actuators,
    ADCS_timer *timer
) {
    this->sensors = sensors;
    this->actuators = actuators;
    this->timer = timer;

    for (const auto &s : sensors) {
        if (Gyroscope* gyro = dynamic_cast<Gyroscope*>(s.second.get())) {
            this->gyro = gyro;
        }
    }
}

void PointingModeController::begin(Eigen::Vector3f desired_attitude, timestamp ramp_time) {
    while (this->gyro->time_until_ready() > 0) {
        this->timer->sleep(this->gyro->time_until_ready());
    }

    measurement initial_vals = this->take_updated_measurements();
    Eigen::Vector3f initial_attitude = initial_vals.vec;
    timestamp start = initial_vals.time_taken;
    timestamp prev_time = start;

    prev_error = Eigen::Vector3f::Zero();
    prev_derivative = Eigen::Vector3f::Zero();
    prev_integral = Eigen::Vector3f::Zero();

    while(true) {
        try {
            measurement m = this->take_updated_measurements();
            timestamp delta_t = m.time_taken - prev_time;
            timestamp since_start = m.time_taken - start;
            prev_time = m.time_taken;

            float ramp_factor = since_start < ramp_time ? ((float) since_start / (float) ramp_time) : 1;
            Eigen::Vector3f ramped_desired_attitude = ramp_factor * (desired_attitude - initial_attitude) + initial_attitude;
            this->update(m.vec, ramped_desired_attitude, delta_t);
        } catch (device_not_ready &_e) {
            this->timer->sleep(this->gyro->time_until_ready());
        }
    }
}

void PointingModeController::update(Eigen::Vector3f current_attitude, Eigen::Vector3f desired_attitude, timestamp delta_t) {
    Eigen::Vector3f kp(0.0002, 0.0002, 0.0002);
    Eigen::Vector3f kd(0.005544, 0.005775, 0.0052472);
    Eigen::Vector3f ki(0.00001, 0.0000096, 0.00001057);
    float N = 1;

    Eigen::Vector3f cur_error = desired_attitude - current_attitude;
    Eigen::Vector3f cur_derivative = (N * kd.cwiseProduct(cur_error - prev_error) + prev_derivative) / (1 + N * (float) delta_t);
    Eigen::Vector3f cur_integral = prev_integral + cur_error * (float) delta_t;

    prev_error = cur_error;
    prev_derivative = cur_derivative;
    prev_integral = cur_integral;

    Eigen::Vector3f desired_torque = -1 * (kp.cwiseProduct(cur_error) + cur_derivative + ki.cwiseProduct(cur_integral));
    
    int num_rws = actuators.size();
    int i = 0;
    Eigen::Matrix3Xf A;
    A.resize(3, num_rws);
    for (const auto &a : actuators) {
        if (Reaction_wheel* rw = dynamic_cast<Reaction_wheel*>(a.second.get())) {
            A.col(i++) << rw->get_axis_of_rotation();
        }
    }

    Eigen::VectorXf rw_torques = A.transpose() * (A * A.transpose()).inverse() * desired_torque;

    // Check each torque to make sure it doesn't exceed the max, if it does
    i = 0;
    for (const auto &a : actuators) {
        if (Reaction_wheel* rw = dynamic_cast<Reaction_wheel*>(a.second.get())) {
            float max_t = rw->get_max_acceleration() * rw->get_inertia_matrix();
            float rw_t = rw_torques[i++];
            if (abs(rw_t) >= max_t) {
                rw_torques = rw_torques * max_t / abs(rw_t) * 0.99;
            }
        }
    }

    i = 0;
    for (const auto &a : actuators) {
        if (Reaction_wheel* rw = dynamic_cast<Reaction_wheel*>(a.second.get())) {
            rw->set_target_state({ 
                rw_torques.coeff(i, 0) / rw->get_inertia_matrix(), 
                rw->get_current_state().velocity, 
                rw->get_current_state().position,
                this->timer->get_time() 
            });
            i++;
        }
    }
}

measurement PointingModeController::take_updated_measurements() {
    try {
        gyro_state state = this->gyro->take_measurement();
        return { state.position, state.time_taken };
    } catch (device_not_ready *e) {
        throw e;
    }
}
