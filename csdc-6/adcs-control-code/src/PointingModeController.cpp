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
}

void PointingModeController::begin(Eigen::Vector3f desired_attitude) {
    //TODO: convert to while and add exit condition
    measurement initial_vals = this->take_updated_measurements();
    timestamp prev_time = initial_vals.time_taken;
    prev_error = desired_attitude - initial_vals.vec;
    prev_integral = Eigen::Vector3f::Zero();
    for (int i = 0; i < 5000; i++) {
        measurement m = this->take_updated_measurements();
        timestamp delta_t = m.time_taken - prev_time;
        prev_time = m.time_taken;
        this->update(m.vec, desired_attitude, delta_t);
    }
}

void PointingModeController::update(Eigen::Vector3f current_attitude, Eigen::Vector3f desired_attitude, timestamp delta_t) {
    Eigen::Vector3f kp(0.0002, 0.0002, 0.0002);
    Eigen::Vector3f kd(0.005544, 0.005775, 0.0052472);
    Eigen::Vector3f ki(0.00001, 0.0000096, 0.00001057);

    Eigen::Vector3f cur_error = desired_attitude - current_attitude;
    Eigen::Vector3f cur_derivative = (cur_error - prev_error) / (float) delta_t;
    Eigen::Vector3f cur_integral = prev_integral + cur_error * (float) delta_t;

    prev_error = cur_error;
    prev_integral = cur_integral;

    Eigen::Vector3f desired_torque = -1 * (kp.cwiseProduct(cur_error) + kd.cwiseProduct(cur_derivative) + ki.cwiseProduct(cur_integral));

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
    // std::cout << "Timestep:" << std::endl << (float) delta_t << std::endl;
    // std::cout << "Error:" << std::endl << cur_error << std::endl;
    // std::cout << "Derivative:" << std::endl << cur_derivative << std::endl;
    // std::cout << "Integral:" << std::endl << cur_integral << std::endl;
    // std::cout << "Torques:" << std::endl << desired_torque << std::endl;

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
    for (const auto &s : sensors) {
        if (Gyroscope* gyro = dynamic_cast<Gyroscope*>(s.second.get())) {
            gyro_state state = gyro->take_measurement();
            return { state.position, state.time_taken };
        }
    }
    return { Eigen::Vector3f::Zero(), 0 };
}
