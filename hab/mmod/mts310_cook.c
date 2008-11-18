
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

#include <math.h>

#include "mts310_cook.h"


float mts310_cook_accel(uint16_t node_id, int axis, uint16_t val)
{
    float ret;

    ret = ((float)val / ACCEL_MPS2_MULTIPLIER) * 10;
    ret = rintf(ret);
    
    return (ret / 10);
} /* mts310_cook_accel() */


unsigned int mts310_cook_voltage(uint16_t val)
{
    return (VOLT_MV_MULTIPLIER / (unsigned int)val);
} /* mts310_cook_voltage() */


float mts310_cook_temperature(uint16_t val)
{
    float logRt;

    logRt = log( (10000 * val) / (1023 - val) );
    
    return ((1 / (A + B * logRt + C * pow(logRt, 3))) - DEG_KELVIN_DIFF);
} /* mts310_cook_temperature() */


unsigned int mts310_cook_light(unsigned int mv, uint16_t val)
{
    return (val * mv / 1023);
} /* mts310_cook_light() */
