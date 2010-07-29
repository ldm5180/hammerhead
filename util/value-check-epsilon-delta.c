
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "internal.h"
#include "bionet-util.h"

int bionet_value_check_epsilon(const bionet_value_t * original_val, 
			       const void * new_val, 
			       const bionet_epsilon_t * epsilon, 
			       bionet_resource_data_type_t data_type) {

    if (NULL == original_val) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_value_check_epsilon_delta: NULL original value passed in.");
	return 1;
    }
    if (NULL == new_val) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_value_check_epsilon_delta: NULL new value passed in.");
	return 1;
    }

    if (epsilon) {
	switch (data_type)
	{
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	{
	    int dval, oval, *nval;
	    nval = (int *)new_val;
	    if (bionet_epsilon_get_binary(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_binary(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (dval <= abs(*nval - oval)) {
		return 1;
	    }
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t dval, oval, *nval;
	    nval = (uint8_t *)new_val;
	    if (bionet_epsilon_get_uint8(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_uint8(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (dval <= abs(*nval - oval)) {
		return 1;
	    }
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t dval, oval, *nval;
	    nval = (int8_t *)new_val;
	    if (bionet_epsilon_get_int8(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_int8(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (dval <= abs(*nval - oval)) {
		return 1;
	    }
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t dval, oval, *nval;
	    nval = (uint16_t *)new_val;
	    if (bionet_epsilon_get_uint16(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_uint16(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (dval <= abs(*nval - oval)) {
		return 1;
	    }
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t dval, oval, *nval;
	    nval = (int16_t *)new_val;
	    if (bionet_epsilon_get_int16(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_int16(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (dval <= abs(*nval - oval)) {
		return 1;
	    }
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t dval, oval, *nval;
	    nval = (uint32_t *)new_val;
	    if (bionet_epsilon_get_uint32(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_uint32(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (dval <= abs(*nval - oval)) {
		return 1;
	    }
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t dval, oval, *nval;
	    nval = (int32_t *)new_val;
	    if (bionet_epsilon_get_int32(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_int32(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (dval <= abs(*nval - oval)) {
		return 1;
	    }
	    break;
	}

        case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float dval, oval, *nval;
	    nval = (float *)new_val;
	    if (bionet_epsilon_get_float(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_float(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (dval <= fabsf(*nval - oval)) {
		return 1;
	    }
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double dval, oval, *nval;
	    nval = (double *)new_val;
	    if (bionet_epsilon_get_double(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_double(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (dval <= fabs(*nval - oval)) {
		return 1;
	    }
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    return 1;

	default:
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "bionet_value_check_epsilon_delta: Invalid data type");
	    return 1;
	}
    }

    return 0;
} /* bionet_value_check_epsilon_delta() */


int bionet_value_check_epsilon_by_value(const bionet_value_t * original_val, 
					const bionet_value_t * new_val, 
					const bionet_epsilon_t * epsilon, 
					bionet_resource_data_type_t data_type) {

    if (NULL == original_val) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_value_check_epsilon_delta: NULL original value passed in.");
	return 1;
    }
    if (NULL == new_val) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_value_check_epsilon_delta: NULL new value passed in.");
	return 1;
    }

    if (epsilon) {
	switch (data_type)
	{
	case BIONET_RESOURCE_DATA_TYPE_BINARY:
	{
	    int dval, oval, nval;
	    if (bionet_epsilon_get_binary(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_binary(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (bionet_value_get_binary(new_val, &nval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from new value");
	    }
	    if (dval <= abs(nval - oval)) {
		return 1;
	    }
	    break;
	}
	case BIONET_RESOURCE_DATA_TYPE_UINT8:
	{
	    uint8_t dval, oval, nval;
	    if (bionet_epsilon_get_uint8(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_uint8(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (bionet_value_get_uint8(new_val, &nval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from new value");
	    }
	    if (dval <= abs(nval - oval)) {
		return 1;
	    }
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_INT8:
	{
	    int8_t dval, oval, nval;
	    if (bionet_epsilon_get_int8(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_int8(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (bionet_value_get_int8(new_val, &nval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from new value");
	    }
	    if (dval <= abs(nval - oval)) {
		return 1;
	    }
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_UINT16:
	{
	    uint16_t dval, oval, nval;
	    if (bionet_epsilon_get_uint16(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_uint16(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (bionet_value_get_uint16(new_val, &nval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from new value");
	    }
	    if (dval <= abs(nval - oval)) {
		return 1;
	    }
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_INT16:
	{
	    int16_t dval, oval, nval;
	    if (bionet_epsilon_get_int16(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_int16(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (bionet_value_get_int16(new_val, &nval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from new value");
	    }
	    if (dval <= abs(nval - oval)) {
		return 1;
	    }
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_UINT32:
	{
	    uint32_t dval, oval, nval;
	    if (bionet_epsilon_get_uint32(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_uint32(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (bionet_value_get_uint32(new_val, &nval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from new value");
	    }
	    if (dval <= abs(nval - oval)) {
		return 1;
	    }
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_INT32:
	{
	    int32_t dval, oval, nval;
	    if (bionet_epsilon_get_int32(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_int32(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (bionet_value_get_int32(new_val, &nval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from new value");
	    }
	    if (dval <= abs(nval - oval)) {
		return 1;
	    }
	    break;
	}

        case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	{
	    float dval, oval, nval;
	    if (bionet_epsilon_get_float(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_float(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (bionet_value_get_float(new_val, &nval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from new value");
	    }
	    if (dval <= fabsf(nval - oval)) {
		return 1;
	    }
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	{
	    double dval, oval, nval;
	    if (bionet_epsilon_get_double(epsilon, &dval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from epsilon");
	    }
	    if (bionet_value_get_double(original_val, &oval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from original value");
	    }
	    if (bionet_value_get_double(new_val, &nval)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		      "bionet_value_check_epsilon_delta: Failed to get content from new value");
	    }
	    if (dval <= fabs(nval - oval)) {
		return 1;
	    }
	    break;
	}

	case BIONET_RESOURCE_DATA_TYPE_STRING:
	    return 1;

	default:
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "bionet_value_check_epsilon_delta: Invalid data type");
	    return 1;
	}
    }

    return 0;
} /* bionet_value_check_epsilon_delta_by_value() */

int bionet_value_check_delta(struct timeval * original_tv,
			     struct timeval * new_tv,
			     struct timeval * delta) {
    struct timeval diff;

    if (delta->tv_sec != 0 || delta->tv_usec != 0) {
	diff = bionet_timeval_subtract(new_tv, original_tv);
	if (0 <= bionet_timeval_compare(&diff, delta)) {
	    return 1;
	}
    }

    return 0;
} /* bionet_value_check_delta() */
