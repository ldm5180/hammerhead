
// Copyright (c) 2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


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

    logRt = log( (10000 * (1023 - val) / val) );
    
    return ((1 / (A + B * logRt + C * pow(logRt, 3))) - DEG_KELVIN_DIFF);
} /* mts310_cook_temperature() */


unsigned int mts310_cook_light(unsigned int mv, uint16_t val)
{
    return (val*100)/(mv);
} /* mts310_cook_light() */
