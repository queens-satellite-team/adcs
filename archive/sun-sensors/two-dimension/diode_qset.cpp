#include <stdlib.h>
#include <math.h>
#include <vector>

#include "diode_qset.h"
#include "PolynomialRegression.h"

#if USE_CALIBRATED_HEADER
#include "calibrated_data.h"
#endif // USE_CALIBRATED_HEADER

using namespace std;

sensor_array::sensor_array() {
    return;
}

#if USE_CALIBRATED_HEADER

void sensor_array::initialize_LUT()
{


    lut.resize(num_diodes);

    lut.at(0).voltage   = two_d_calibrated::voltage_0;
    vector_x_scalar(lut.at(0).voltage, max_voltage_expected)

    lut.at(0).angle_top = two_d_calibrated::angle_top_0;
    lut.at(0).angle_bot = two_d_calibrated::angle_bot_0;

    lut.at(1).voltage   = two_d_calibrated::voltage_1;
    vector_x_scalar(lut.at(1).voltage, max_voltage_expected)

    lut.at(1).angle_top = two_d_calibrated::angle_top_1;
    lut.at(1).angle_bot = two_d_calibrated::angle_bot_1;

    lut.at(2).voltage   = two_d_calibrated::voltage_2;
    vector_x_scalar(lut.at(2).voltage, max_voltage_expected)

    lut.at(2).angle_top = two_d_calibrated::angle_top_2;
    lut.at(2).angle_bot = two_d_calibrated::angle_bot_2;

    lut.at(3).voltage   = two_d_calibrated::voltage_3;
    vector_x_scalar(lut.at(3).voltage, max_voltage_expected)

    lut.at(3).angle_top = two_d_calibrated::angle_top_3;
    lut.at(3).angle_bot = two_d_calibrated::angle_bot_3;
}

#else

void sensor_array::calibrate_one_axis (diode_data raw_values)
{
    curve_fit(raw_values);
    find_shift(raw_values);
    generate_lut();
    return;
}



void sensor_array::curve_fit(diode_data raw_values) { // include a size for raw_values
    int num_cal_pnts    = raw_values.angles.size();

    vector<ret_curve_fit> ret;
    ret.resize(num_diodes);
    find_noise_floor(raw_values);

// ************************************** Loop through each diode ***********************************
    for(int diode = x_p; diode < num_diodes; diode++) {
// ****************************************** Setup Variables ***************************************
        int first_index             = 0;
        int last_index              = 0;
        int num_spots_to_shift      = 0;
        double max_val              = 0;
        int max_val_index           = 0;
        // float average_error         = 0;
        // int iterations              = 0;
        // float prev_average_error    = 10;
        // int centre                  = 0;
        int centre_index            = 0;

        vector<double> left_data;
        vector<double> left_angle_rad;
        vector<double> right_data;
        vector<double> right_angle_rad;

        diode_data pos_data;
        pos_data.voltage.resize(1);

// ************************************** Extract Above Noise Floor *********************************
        for (int j = 0; j < num_cal_pnts; j++) {
            // Extract data above the noise floor and find the centre value
            if (raw_values.voltage[diode][j] > noise_floor) {
                pos_data.voltage[0].push_back(raw_values.voltage[diode][j]);
                pos_data.angles.push_back(raw_values.angles[j]);
                if (pos_data.voltage[0].back() > max_val) {
                    max_val = pos_data.voltage[0].back();
                    max_val_index = j;
                }
            } else {
                // Make the first value on either side of the real data 0
                if (raw_values.voltage[diode][get_index_wrap(j+1,raw_values.voltage[diode].size())] > noise_floor) {
                    first_index = get_index_wrap(j+1,raw_values.voltage[diode].size());
                } else if (raw_values.voltage[diode][get_index_wrap(j-1,raw_values.voltage[diode].size())] > noise_floor ) {
                    last_index = get_index_wrap(j-1,raw_values.voltage[diode].size());
                    // pos_data.voltage[0].push_back(0);
                    // pos_data.angles.push_back(raw_values.angles[j]);
                }

                // We will need to shift the data to "unwrap" it. We will shift it by the number of points before the last "positive" point.
                if (raw_values.voltage[diode][get_index_wrap(j-1,num_cal_pnts)] > noise_floor) {
                    num_spots_to_shift = pos_data.voltage[0].size();
                }
            }
        } // end for

        centre_index = get_index_wrap(first_index + (last_index - first_index)/2, raw_values.voltage[diode].size());

        ret[diode].cen_val_angle = raw_values.angles.at(centre_index);
        ret[diode].max_val_idx   = max_val_index;
        ret[diode].max_val       = max_val;

// *************************************************** Centre Data ***************************************************
        diode_data shifted_pos;
        shifted_pos.voltage.resize(1);

        for (int j = 0; j < pos_data.voltage[0].size(); j++) {
            // Shifted index is wrapped around incase of overflow
            int shfited_index = get_index_wrap(j + num_spots_to_shift,pos_data.voltage[0].size());

            // Voltage (data) is normalized before being pushed back
            shifted_pos.voltage[0].push_back(pos_data.voltage[0][shfited_index] / max_val);
            shifted_pos.angles.push_back(pos_data.angles[shfited_index]);

            // Max value index is different for shifted data
            if (shifted_pos.voltage[0][j] == 1)
                max_val_index = j;
        }

// ********************************* Split curve in half and change to x = voltage ***********************************
        /** While we split the curve, we will also be shifting the angles and converting them to radians.
         *  The angles will be shifted such that the maximum voltage is at 0 RAD. 
         * 
         *  In order to properly convert the angles to this new scheme, we must simply take off the angle 
         *  the max value is currently at, such that the max value is at angle 0. However, there is 
         *  occasionally a scenario where part of the data wraps to either negative or positive values. 
         *  This happens because we only measure angles to +/-180 degrees, so any larger/smaller angles
         *  will appear to wrap around. The first to if cases cover this to properly map from -90 to 90 degrees.
         */
        for (int j = 0; j < shifted_pos.angles.size(); j++) {
            // Calculate angle shift as described above
            double angle_shift = 0;
            if ( ( shifted_pos.angles[max_val_index] > 90 ) && ( shifted_pos.angles[j] < -90 ) )
                angle_shift = 360 - shifted_pos.angles[max_val_index];
            else if ( (shifted_pos.angles[max_val_index] < -90) && ( shifted_pos.angles[j] > 0 ) )
                angle_shift = -360 - shifted_pos.angles[max_val_index];
            else
                angle_shift = - shifted_pos.angles[max_val_index];

            // Separate left side of the data
            if (j <= max_val_index) {
                left_data.push_back(shifted_pos.voltage[0][j]);
                left_angle_rad.push_back( (shifted_pos.angles[j] + angle_shift) );
            }

            // Separate right side of the data
            if (j >= max_val_index) {
                right_data.push_back(shifted_pos.voltage[0][j]);
                right_angle_rad.push_back( (shifted_pos.angles[j] + angle_shift) );
            }
        }

// ******************************************* Curve fit each side ***********************************
        /** This curve fitting is done with a least squares polynomial fit. The code was written by 
         *  Chris Engelsma, and is used under the MIT License.
         */
        PolynomialRegression<double> least_squares;

        // Right side
        vector<double> coeff_right(fit_order);
        least_squares.fitIt(right_data, right_angle_rad, fit_order, coeff_right);

        // Left Side
        vector<double> coeff_left(fit_order);
        least_squares.fitIt(left_data, left_angle_rad, fit_order, coeff_left);

        // Save the order to the ret variable
        for (int j = 0; j <= fit_order; j++) {
            ret[diode].top_coeffs.push_back(coeff_right.at(j));
            ret[diode].bot_coeffs.push_back(coeff_left.at(j));
        }
    } // for (each diode)

    this->fitted_curves = ret;
    return;
} // curve_fit

// TODO: clean up this code
void sensor_array::find_noise_floor (diode_data raw_values) {
    int num_cal_pnts = raw_values.angles.size();
    // find max value location
    int max_index = 0;
    double max_val = 0;
    for (int i = 0; i < raw_values.voltage.at(1).size(); i++) {
        if (max_val < raw_values.voltage.at(1).at(i)) {
            max_val = raw_values.voltage.at(1).at(i);
            max_index = i;
        }
    }
    // Find opposite
    int opposite_index = get_index_wrap(max_index - num_cal_pnts/2, num_cal_pnts);

    // average 10 points around opposite
    double sum = 0;
    for (int i = opposite_index - noise_floor_width; i <= opposite_index + noise_floor_width; i++) {
        sum += raw_values.voltage[1][get_index_wrap(i,num_cal_pnts)];
    }

    double average = sum / (2*noise_floor_width + 1);

    // return 
    this->noise_floor =  average * noise_floor_moa;
    return;
}

void sensor_array::find_shift(diode_data raw_values) {
    vector<double> shifts(num_diodes);

    for (int diode = 0; diode < num_diodes; diode++) {
        // for (int j = 0; j < )
        // Double for ratio, truncates in second line
        double start_idx_from_centre    = raw_values.angles.size() * (3.0/16.0);
        double end_idx_from_centre      = raw_values.angles.size() * (1.0/16.0);

        int unwrapped_start = fitted_curves.at(diode).max_val_idx - start_idx_from_centre;
        int unwrapped_end   = fitted_curves.at(diode).max_val_idx - end_idx_from_centre;

        int start_idx   = get_index_wrap( unwrapped_start, raw_values.angles.size());
        int end_idx     = get_index_wrap( unwrapped_end, raw_values.angles.size()); // Should make this ratio a definition?

        int num_points  = 0;
        if (start_idx > end_idx)
            num_points = end_idx + raw_values.angles.size() - start_idx;
        else
            num_points = end_idx  - start_idx;

        double diff = 0;
        for (int j = 0; j < num_points; j++) {
            int idx = get_index_wrap(start_idx + j, raw_values.angles.size());
            double voltage   = raw_values.voltage.at(diode).at(idx) / fitted_curves.at(diode).max_val;
            double angle_bot = centered_func(voltage, diode, false);

            diff += raw_values.angles.at(idx) - angle_bot;
        }
        diff /= num_points;

        shifts.at(diode) = diff;
    }
    
    this->shifts_from_centre = shifts;
    return;
}

void sensor_array::generate_lut() {
// ************************************************** Setup ******************************************
    vector<LUT> luts(num_diodes);

    for (int diode = 0; diode < num_diodes; diode++) {
// ********************************* Generate the centered part of the LUT ***************************
        double angle_top    = 0;
        double angle_bot    = 0;
        double voltage      = 0;

        int num_samples = ceil(1.0 / LUT_precision) + 10; // Why is there a + 10

        for (int j = 0; j < num_samples; j++) {
            voltage   = j*LUT_precision;
            angle_top = centered_func(voltage, diode, true);
            angle_bot = centered_func(voltage, diode, false);

// ********************************************* Shift the angles ************************************
            // The max voltage should be at angle = 0 in the centered data, so just shift all the angles
            // by the "max_val_angle" value

            if (angle_top > angle_bot && voltage >= noise_floor ) {
                luts.at(diode).angle_top.push_back(angle_wrap(angle_top + this->shifts_from_centre.at(diode)));
                luts.at(diode).angle_bot.push_back(angle_wrap(angle_bot + this->shifts_from_centre.at(diode)));
                luts.at(diode).voltage.push_back(voltage * this->fitted_curves.at(diode).max_val);
            }
        } // for each LUT sample point
    } // for each diode

    this->lut = luts;
    return;
} // sensor_array::generate_lut()

double sensor_array::centered_func(double voltage, int diode_num, bool top) {
    vector<double> coeffs;
    double angle = 0;

    if (top)
        coeffs = this->fitted_curves.at(diode_num).top_coeffs;
    else    
        coeffs = this->fitted_curves.at(diode_num).bot_coeffs;

    for (int i = 0; i < coeffs.size(); i++) {
        angle += coeffs.at(i) * pow(voltage, i);
    }

    return angle;
}

#endif // USE_CALIBRATED_HEADER

// Eventually this should be converted to output the angle for each individual diode - will leave like this for now
double sensor_array::get_angle (vector<double> voltages) {
    if (voltages.size() != num_diodes) {
        // handle error
    }

    int hgh_diode_idx = 0;
    int low_diode_idx = 0;

    // Find the highest two voltages -> the sun should only ever face two diodes at max, so we
    // can just look at the top two voltages, the rest should be dark
    for (int i = 0; i < num_diodes; i++) {
        if (voltages.at(i) > voltages.at(hgh_diode_idx))
            hgh_diode_idx = i;
    }

    if (hgh_diode_idx == low_diode_idx)
        low_diode_idx++;
    for (int i = 0; i < num_diodes; i++) {
        if (i != hgh_diode_idx && voltages.at(i) > voltages.at(low_diode_idx))
            low_diode_idx = i;
    }

// TODO --------------------------  We should consider two cases, one where the values are both reasonable, and one where the 
//                                  the smaller voltage is close to 0. We could have some noise floor that we calculated during 
//                                  calibration?

    // The two diodes should be next to each other
    if (abs_val(hgh_diode_idx - low_diode_idx) == 2) {
        // handle error
    }

    double fst_diode_idx = -1;
    double scd_diode_idx = -1;

    // determine Which is first and second in terms of order
    if ((hgh_diode_idx == 0 && low_diode_idx == 3) ||
        (low_diode_idx == 0 && hgh_diode_idx == 3)) {
        fst_diode_idx = 3;
        scd_diode_idx = 0;
    } else if (hgh_diode_idx > low_diode_idx) {
        fst_diode_idx = low_diode_idx;
        scd_diode_idx = hgh_diode_idx;
    } else {
        scd_diode_idx = low_diode_idx;
        fst_diode_idx = hgh_diode_idx;
    }

#if SEARCH_METHOD == 0
    // check the lookup table.
    int fst_diode_lut_idx = search_lut(voltages.at(fst_diode_idx), fst_diode_idx);
    int scd_diode_lut_idx = search_lut(voltages.at(scd_diode_idx), scd_diode_idx);

    // The actual angle should be between the two diodes, so we should choose
    // the side of each diode that should be facing the sun to get the angle
    double fst_angle = this->lut.at(fst_diode_idx).angle_bot.at(fst_diode_lut_idx);
    double scd_angle = this->lut.at(scd_diode_idx).angle_top.at(scd_diode_lut_idx);

#ifdef DEBUG
    // cout << "\t\tfirst(bot) idx:\t" << fst_diode_idx << "\tsecond(top) idx:\t" << scd_diode_idx << endl;
    // cout << "fst_angle:\t" << fst_angle << endl << "scd_angle:\t" << scd_angle << endl;
#endif // DEBUG
#endif // SEARCH_METHOD == 0

#if SEARCH_METHOD == 1
    // Devide by the maximum value, centered_func takes in a normalized voltage
    double fst_normalized_voltage = voltages.at(fst_diode_idx) / this->fitted_curves.at(fst_diode_idx).max_val;
    double scd_normalized_voltage = voltages.at(scd_diode_idx) / this->fitted_curves.at(scd_diode_idx).max_val;

    // calculate the angle
    double fst_centered_angle = centered_func(fst_normalized_voltage, fst_diode_idx, true);
    double scd_centered_angle = centered_func(scd_normalized_voltage, scd_diode_idx, false);

    // angle is centered about 0, shift it by the appropraite amount
    double fst_angle = angle_wrap(fst_centered_angle + fitted_curves.at(fst_diode_idx).max_val_angle);
    double scd_angle = angle_wrap(scd_centered_angle + fitted_curves.at(scd_diode_idx).max_val_angle);
#endif

    /** TODO  ----- Check if these are within reasonable values?? There are a couple ways we could validate the result from the LUT:
     * 1. check if the two angles are within some acceptable range
     * 2. check if the oposite side (top/bottom) LUT gives better results
     * 3. Check with team for more ideas
     */ 

    // return the average of these angles
    return (fst_angle + scd_angle) / 2;
}

#if SEARCH_METHOD == 0
// Binary search for the value. 
int sensor_array::search_lut(double voltage, int diode_num) {
    vector<double> LUT = this->lut.at(diode_num).voltage;

    int left    = 0;
    int mid     = 0;
    int right   = LUT.size() - 1;

    while (left <= right) {
        mid = left + (right - left) / 2;

        // Check if the voltage is between the midpoint and the next value
        if (LUT.at(mid) <= voltage && voltage <= LUT.at(mid+1)) {
            double l = voltage - LUT.at(mid);
            double r = LUT.at(mid+1) - voltage;

            if (r > l)
                return mid + 1;
            else
                return mid;
        }

        // check if the voltage is between the midpoint and the previous value
        if (LUT.at(mid) > voltage && voltage > LUT.at(mid-1)) {
            double l = voltage - LUT.at(mid-1);
            double r = LUT.at(mid) - voltage;

            if (r > l)
                return mid;
            else
                return mid - 1;
        }

        // if the voltage wasn't found, cut the search area in half
        if (LUT.at(mid) < voltage)
            left = mid;
        else
            right = mid;
    }

    return -1;
}
#endif
