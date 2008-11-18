
//
// Copyright (C) 2004-2008, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of GNU General Public License version 2, as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//

#ifndef MTS_COOK_H
#define MTS310_COOK_H

#include <stdint.h>
#include <math.h>

/**
 * Cook accelerometer sensor reading to m/s^2
 *
 * Accel = val - ACCEL_MEDIAN_VAL / ACCEL_MP2S_MULTIPLIER
 *
 * @param[in] node_id ID of Node reporting data
 * @param[in] axis #X_AXIS or #Y_AXIS
 * @param[in] val Value read from the accelerometer
 *
 * @return Acceleration in meters per second squared (m/s^2)
 */
#define X_AXIS 0
#define Y_AXIS 1
#define ACCEL_MEDIAN_VAL      (450)
#define ACCEL_MPS2_MULTIPLIER (5.1)
float mts310_cook_accel(uint16_t node_id, int axis, uint16_t val);


/**
 * Cook voltage sensor reading to mV
 *
 * mV = VOLT_MV_MULTIPLIER / val
 *
 * @param[in] val Value read from voltage sensor
 *
 * @return Voltage in millivolts (mV)
 */
#define VOLT_MV_MULTIPLIER (1252352)
#define VOLT_V_MULTIPLER   (1252.352)
unsigned int mts310_cook_voltage(uint16_t val);


/**
 * Cook thermistor reading to degrees C
 *
 * Rt = (10000 * val) / (1023 - val)
 * degC = 1/(A + B * log(Rt) + C * pow(log(Rt),3)) - DEG_KELVIN_DIFF
 * 
 * @param[in] val Value read from thermistor
 * 
 * @return Temperature in degrees celsius (C)
 */
#define A (0.001307050)
#define B (0.000214381)
#define C (0.000000093)
#define DEG_KELVIN_DIFF (273.15)
float mts310_cook_temperature(uint16_t val);


/**
 * Cook photo sensor reading to mV intensity
 *
 * Vbat = current battery voltage level (mV) 
 * mVlight = val * Vbat / 1023
 * 
 * @param[in] mv Cooked value from voltage sensor (see mts310_cook_voltage())
 * @param[in] val Value read from photo sensor
 * 
 * @return Light intensity in millivolts (mV)
 */
unsigned int mts310_cook_light(unsigned int mv, uint16_t val);

#endif /* MTS_COOK_H */
