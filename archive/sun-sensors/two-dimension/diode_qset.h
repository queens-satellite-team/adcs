#pragma once

// #include <Arduino.h>
#include <math.h>
#include <vector>

using namespace std;

// general definitions
#define noise_floor_width   5       // These values seem to work really well. Change at own risk.
#define noise_floor_moa     1.25    // These values seem to work really well. Change at own risk.
#define num_diodes          4

// Curve fitting definitions
#define fit_order       8           // These values seem to work really well. Change at own risk.
#define LUT_precision   0.01

// Math macros
#define get_index_wrap(index, num_indices)  ((index + num_indices) % num_indices)
#define abs_val(x)                          sqrt(pow(x,2))
#define angle_wrap(angle)                   (fmod((angle + 540), (360)) - 180)
#define vector_x_scalar(vector, scalar)     for (int i = 0; i < vector.size(); i++) {vector.at(i) *= scalar;}

// shifting definitions
#define max_shift_err           0.01
#define max_shift_iterations    100

// REPLACE THIS VALUE WITH THE EXPECTED MAXIMUM VOLTAGE YOU WILL RECIEVE
#define max_voltage_expected    4



/** angle searching macros
 * possible values:
 * 
 *      0:  LUT method
 *      1:  function method
 * 
 */
#define SEARCH_METHOD 0
#define USE_CALIBRATED_HEADER 1

typedef struct {
    vector<double>           angles;
    vector<vector<double>>   voltage;
} diode_data;

typedef struct {
    vector<double>  bot_coeffs;
    vector<double>  top_coeffs;
    double  cen_val_angle;
    double  max_val_idx;
    double  max_val;
} ret_curve_fit;

typedef struct {
    vector<double>  voltage;
    vector<double>  angle_top;
    vector<double>  angle_bot;
} LUT;

class sensor_array {
public:
    /**
     * Constructor/destructor for sensor_array
     */
    sensor_array ();
    ~sensor_array () { return; }

    /**
     * accessors/setters
     */
    double get_angle (vector<double> voltages);

#if USE_CALIBRATED_HEADER
    void initialize_LUT();
#else
    /**
     * @brief This function runs a callibration routine for the
     * 1 axis:
     *      1.  rotate 360 degrees, taking a reading for each diode
     *          at _____ intervlas
     *      2.  create a LUT for the initial curves
     *      3.  Fit each curve around the 180 degrees where each 
     *          photodiode is getting a reading
     *      4.  extrapolate the curves and create a secondary set 
     *          of LUTs
     */
    void calibrate_one_axis (diode_data raw_values);
#endif // USE_CALIBRATED_HEADER

private:
    int     num_data_points;
    double  noise_floor;

    vector<ret_curve_fit>   fitted_curves;
    vector<double>          shifts_from_centre;
    vector<LUT>             lut;

#if USE_CALIBRATED_HEADER == 0
    void    curve_fit(diode_data raw_values);
    void    find_noise_floor (diode_data raw_values);
    void    find_shift(diode_data raw_values);
    void    generate_lut();
    double  centered_func(double voltage, int diode_num, bool top);
#endif // USE_CALIBRATED_HEADER == 0

#if SEARCH_METHOD == 0
    int search_lut(double voltage, int diode_num);
#endif
};
