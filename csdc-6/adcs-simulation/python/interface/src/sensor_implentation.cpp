
#include "sim_interface.hpp"

Accelerometer::take_measurement()
{
    return {1};
}

void Accelerometer::sim_set_current_vals(float** new_vals, uint32_t axes, uint32_t num_sensors) {
    return;
}

Gyroscope::take_measurement()
{
    return {1};
}

void Gyroscope::sim_set_current_vals(float** new_vals, uint32_t axes, uint32_t num_sensors) {
    return;
}
