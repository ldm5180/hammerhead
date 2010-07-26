
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet-util.h"
#include "internal.h"




int bionet_resource_set(
    bionet_resource_t *resource, 
    const bionet_value_t *content, 
    const struct timeval *timestamp
) {
    bionet_value_t *value;
    bionet_datapoint_t *dp;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set(): NULL Resource passed in");
        return -1;
    }

    if (content == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set(): NULL content (value) passed in");
        return -1;
    }

    if (bionet_resource_get_num_datapoints(resource) > 0) {
	if (0 == bionet_value_check_epsilon_delta_by_value(bionet_datapoint_get_value(BIONET_RESOURCE_GET_DATAPOINT(resource)),
							   content,
							   bionet_resource_get_epsilon(resource),
							   bionet_resource_get_data_type(resource),
							   bionet_datapoint_get_timestamp(BIONET_RESOURCE_GET_DATAPOINT(resource)),
							   timestamp,
							   bionet_resource_get_delta(resource))) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		  "bionet_resource_set(): New content %s for %s does not pass delta/epsilon filter. Not publishing.",
		  bionet_value_to_str(content),
		  bionet_resource_get_name(resource));
	    return 0;
	}	    
    }

    value = bionet_value_dup(resource, content);
    if (value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set(): error duplicating content");
        return -1;
    }

    dp = bionet_datapoint_new(resource, value, timestamp);
    if (dp == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set(): error creating datapoint");
        return -1;
    }

    bionet_resource_remove_datapoint_by_index(resource, 0);
    bionet_resource_add_datapoint(resource, dp);

    return 0;
}




int bionet_resource_set_binary(bionet_resource_t *resource, 
			       int content, 
			       const struct timeval *timestamp) 
{
    bionet_datapoint_t *d;
    bionet_value_t *value;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set(): NULL Resource passed in");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
	value = bionet_value_new_binary(resource, content);
	if (NULL == value)
	{
	    return -1;
	}

        d = bionet_datapoint_new(resource, value, timestamp);
        if (d == NULL) {
            // an error has been logged
            return -1;
        }

        bionet_resource_add_datapoint(resource, d);
    }
    else
    {
	value = bionet_datapoint_get_value(d);
	if (NULL == value)
	{
	    return -1;
	}

	if (bionet_resource_get_num_datapoints(resource) > 0) {
	    if (0 == bionet_value_check_epsilon_delta(bionet_datapoint_get_value(d),
						      (void *)&content,
						      bionet_resource_get_epsilon(resource),
						      bionet_resource_get_data_type(resource),
						      bionet_datapoint_get_timestamp(d),
						      timestamp,
						      bionet_resource_get_delta(resource))) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		      "bionet_resource_set_binary(): New content %s for %s does not pass delta/epsilon filter. Not publishing.",
		      bionet_value_to_str(value),
		      bionet_resource_get_name(resource));
		return 0;
	    }	    
	}

	bionet_value_set_binary(value, content);
	bionet_datapoint_set_timestamp(d, timestamp);
    }

    return 0;
} /* bionet_resource_set_binary() */


int bionet_resource_set_uint8(bionet_resource_t *resource, 
			      uint8_t content, 
			      const struct timeval *timestamp) 
{
    bionet_datapoint_t *d;
    bionet_value_t *value;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set(): NULL Resource passed in");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
	value = bionet_value_new_uint8(resource, content);
	if (NULL == value)
	{
	    return -1;
	}

        d = bionet_datapoint_new(resource, value, timestamp);
        if (d == NULL) {
            // an error has been logged
            return -1;
        }

        bionet_resource_add_datapoint(resource, d);
    }
    else
    {
	value = bionet_datapoint_get_value(d);
	if (NULL == value)
	{
	    return -1;
	}

	if (bionet_resource_get_num_datapoints(resource) > 0) {
	    if (0 == bionet_value_check_epsilon_delta(bionet_datapoint_get_value(d),
						      (void *)&content,
						      bionet_resource_get_epsilon(resource),
						      bionet_resource_get_data_type(resource),
						      bionet_datapoint_get_timestamp(d),
						      timestamp,
						      bionet_resource_get_delta(resource))) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		      "bionet_resource_set_uint8(): New content %s for %s does not pass delta/epsilon filter. Not publishing.",
		      bionet_value_to_str(value),
		      bionet_resource_get_name(resource));
		return 0;
	    }	    
	}

	bionet_value_set_uint8(value, content);
	bionet_datapoint_set_timestamp(d, timestamp);
    }

    return 0;
} /* bionet_resource_set_uint8() */


int bionet_resource_set_int8(bionet_resource_t *resource, 
			     int8_t content, 
			     const struct timeval *timestamp) 
{
    bionet_datapoint_t *d;
    bionet_value_t *value;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set(): NULL Resource passed in");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
	value = bionet_value_new_int8(resource, content);
	if (NULL == value)
	{
	    return -1;
	}

        d = bionet_datapoint_new(resource, value, timestamp);
        if (d == NULL) {
            // an error has been logged
            return -1;
        }

        bionet_resource_add_datapoint(resource, d);
    }
    else
    {
	value = bionet_datapoint_get_value(d);
	if (NULL == value)
	{
	    return -1;
	}

	if (bionet_resource_get_num_datapoints(resource) > 0) {
	    if (0 == bionet_value_check_epsilon_delta(bionet_datapoint_get_value(d),
						      (void *)&content,
						      bionet_resource_get_epsilon(resource),
						      bionet_resource_get_data_type(resource),
						      bionet_datapoint_get_timestamp(d),
						      timestamp,
						      bionet_resource_get_delta(resource))) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		      "bionet_resource_set_int8(): New content %s for %s does not pass delta/epsilon filter. Not publishing.",
		      bionet_value_to_str(value),
		      bionet_resource_get_name(resource));
		return 0;
	    }	    
	}

	bionet_value_set_int8(value, content);
	bionet_datapoint_set_timestamp(d, timestamp);
    }

    return 0;
} /* bionet_resource_set_int8() */


int bionet_resource_set_uint16(bionet_resource_t *resource, 
			       uint16_t content, 
			       const struct timeval *timestamp) 
{
    bionet_datapoint_t *d;
    bionet_value_t *value;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set(): NULL Resource passed in");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
	value = bionet_value_new_uint16(resource, content);
	if (NULL == value)
	{
	    return -1;
	}

        d = bionet_datapoint_new(resource, value, timestamp);
        if (d == NULL) {
            // an error has been logged
            return -1;
        }

        bionet_resource_add_datapoint(resource, d);
    }
    else
    {
	value = bionet_datapoint_get_value(d);
	if (NULL == value)
	{
	    return -1;
	}

	if (bionet_resource_get_num_datapoints(resource) > 0) {
	    if (0 == bionet_value_check_epsilon_delta(bionet_datapoint_get_value(d),
						      (void *)&content,
						      bionet_resource_get_epsilon(resource),
						      bionet_resource_get_data_type(resource),
						      bionet_datapoint_get_timestamp(d),
						      timestamp,
						      bionet_resource_get_delta(resource))) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		      "bionet_resource_set_uint16(): New content %s for %s does not pass delta/epsilon filter. Not publishing.",
		      bionet_value_to_str(value),
		      bionet_resource_get_name(resource));
		return 0;
	    }	    
	}

	bionet_value_set_uint16(value, content);
	bionet_datapoint_set_timestamp(d, timestamp);
    }

    return 0;
} /* bionet_resource_set_uint16() */


int bionet_resource_set_int16(bionet_resource_t *resource, 
			      int16_t content, 
			      const struct timeval *timestamp) 
{
    bionet_datapoint_t *d;
    bionet_value_t *value;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set(): NULL Resource passed in");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
	value = bionet_value_new_int16(resource, content);
	if (NULL == value)
	{
	    return -1;
	}

        d = bionet_datapoint_new(resource, value, timestamp);
        if (d == NULL) {
            // an error has been logged
            return -1;
        }

        bionet_resource_add_datapoint(resource, d);
    }
    else
    {
	value = bionet_datapoint_get_value(d);
	if (NULL == value)
	{
	    return -1;
	}

	if (bionet_resource_get_num_datapoints(resource) > 0) {
	    if (0 == bionet_value_check_epsilon_delta(bionet_datapoint_get_value(d),
						      (void *)&content,
						      bionet_resource_get_epsilon(resource),
						      bionet_resource_get_data_type(resource),
						      bionet_datapoint_get_timestamp(d),
						      timestamp,
						      bionet_resource_get_delta(resource))) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		      "bionet_resource_set_int16(): New content %s for %s does not pass delta/epsilon filter. Not publishing.",
		      bionet_value_to_str(value),
		      bionet_resource_get_name(resource));
		return 0;
	    }	    
	}

	bionet_value_set_int16(value, content);
	bionet_datapoint_set_timestamp(d, timestamp);
    }

    return 0;
} /* bionet_resource_set_int16() */


int bionet_resource_set_uint32(bionet_resource_t *resource, 
			       uint32_t content, 
			       const struct timeval *timestamp) 
{
    bionet_datapoint_t *d;
    bionet_value_t *value;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set(): NULL Resource passed in");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
	value = bionet_value_new_uint32(resource, content);
	if (NULL == value)
	{
	    return -1;
	}

        d = bionet_datapoint_new(resource, value, timestamp);
        if (d == NULL) {
            // an error has been logged
            return -1;
        }

        bionet_resource_add_datapoint(resource, d);
    }
    else
    {
	value = bionet_datapoint_get_value(d);
	if (NULL == value)
	{
	    return -1;
	}

	if (bionet_resource_get_num_datapoints(resource) > 0) {
	    if (0 == bionet_value_check_epsilon_delta(bionet_datapoint_get_value(d),
						      (void *)&content,
						      bionet_resource_get_epsilon(resource),
						      bionet_resource_get_data_type(resource),
						      bionet_datapoint_get_timestamp(d),
						      timestamp,
						      bionet_resource_get_delta(resource))) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		      "bionet_resource_set_uint32(): New content %s for %s does not pass delta/epsilon filter. Not publishing.",
		      bionet_value_to_str(value),
		      bionet_resource_get_name(resource));
		return 0;
	    }	    
	}

	bionet_value_set_uint32(value, content);
	bionet_datapoint_set_timestamp(d, timestamp);
    }

    return 0;
} /* bionet_resource_set_uint32() */


int bionet_resource_set_int32(bionet_resource_t *resource, 
			      int32_t content, 
			      const struct timeval *timestamp) 
{
    bionet_datapoint_t *d;
    bionet_value_t *value;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set(): NULL Resource passed in");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
	value = bionet_value_new_int32(resource, content);
	if (NULL == value)
	{
	    return -1;
	}

        d = bionet_datapoint_new(resource, value, timestamp);
        if (d == NULL) {
            // an error has been logged
            return -1;
        }

        bionet_resource_add_datapoint(resource, d);
    }
    else
    {
	value = bionet_datapoint_get_value(d);
	if (NULL == value)
	{
	    return -1;
	}

	if (bionet_resource_get_num_datapoints(resource) > 0) {
	    if (0 == bionet_value_check_epsilon_delta(bionet_datapoint_get_value(d),
						      (void *)&content,
						      bionet_resource_get_epsilon(resource),
						      bionet_resource_get_data_type(resource),
						      bionet_datapoint_get_timestamp(d),
						      timestamp,
						      bionet_resource_get_delta(resource))) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		      "bionet_resource_set_int32(): New content %s for %s does not pass delta/epsilon filter. Not publishing.",
		      bionet_value_to_str(value),
		      bionet_resource_get_name(resource));
		return 0;
	    }	    
	}

	bionet_value_set_int32(value, content);
	bionet_datapoint_set_timestamp(d, timestamp);
    }

    return 0;
} /* bionet_resource_set_int32() */


int bionet_resource_set_float(bionet_resource_t *resource, 
			      float content, 
			      const struct timeval *timestamp) 
{
    bionet_datapoint_t *d;
    bionet_value_t *value;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set(): NULL Resource passed in");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
	value = bionet_value_new_float(resource, content);
	if (NULL == value)
	{
	    return -1;
	}

        d = bionet_datapoint_new(resource, value, timestamp);
        if (d == NULL) {
            // an error has been logged
            return -1;
        }

        bionet_resource_add_datapoint(resource, d);
    }
    else
    {
	value = bionet_datapoint_get_value(d);
	if (NULL == value)
	{
	    return -1;
	}

	if (bionet_resource_get_num_datapoints(resource) > 0) {
	    if (0 == bionet_value_check_epsilon_delta(bionet_datapoint_get_value(d),
						      (void *)&content,
						      bionet_resource_get_epsilon(resource),
						      bionet_resource_get_data_type(resource),
						      bionet_datapoint_get_timestamp(d),
						      timestamp,
						      bionet_resource_get_delta(resource))) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		      "bionet_resource_set_float(): New content %s for %s does not pass delta/epsilon filter. Not publishing.",
		      bionet_value_to_str(value),
		      bionet_resource_get_name(resource));
		return 0;
	    }	    
	}

	bionet_value_set_float(value, content);
	bionet_datapoint_set_timestamp(d, timestamp);
    }

    return 0;
} /* bionet_resource_set_float() */


int bionet_resource_set_double(bionet_resource_t *resource, 
			       double content, 
			       const struct timeval *timestamp) 
{
    bionet_datapoint_t *d;
    bionet_value_t *value;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set(): NULL Resource passed in");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
	value = bionet_value_new_double(resource, content);
	if (NULL == value)
	{
	    return -1;
	}

        d = bionet_datapoint_new(resource, value, timestamp);
        if (d == NULL) {
            // an error has been logged
            return -1;
        }

        bionet_resource_add_datapoint(resource, d);
    }
    else
    {
	value = bionet_datapoint_get_value(d);
	if (NULL == value)
	{
	    return -1;
	}

	if (bionet_resource_get_num_datapoints(resource) > 0) {
	    if (0 == bionet_value_check_epsilon_delta(bionet_datapoint_get_value(d),
						      (void *)&content,
						      bionet_resource_get_epsilon(resource),
						      bionet_resource_get_data_type(resource),
						      bionet_datapoint_get_timestamp(d),
						      timestamp,
						      bionet_resource_get_delta(resource))) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		      "bionet_resource_set_double(): New content %s for %s does not pass delta/epsilon filter. Not publishing.",
		      bionet_value_to_str(value),
		      bionet_resource_get_name(resource));
		return 0;
	    }	    
	}

	bionet_value_set_double(value, content);
	bionet_datapoint_set_timestamp(d, timestamp);
    }

    return 0;
} /* bionet_resource_set_double() */


int bionet_resource_set_str(bionet_resource_t *resource, 
			    const char * content, 
			    const struct timeval *timestamp) 
{
    bionet_datapoint_t *d;
    bionet_value_t *value;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_set(): NULL Resource passed in");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
	value = bionet_value_new_str(resource, content);
	if (NULL == value)
	{
	    return -1;
	}

        d = bionet_datapoint_new(resource, value, timestamp);
        if (d == NULL) {
            // an error has been logged
            return -1;
        }

        bionet_resource_add_datapoint(resource, d);
    }
    else
    {
	value = bionet_datapoint_get_value(d);
	if (NULL == value)
	{
	    return -1;
	}

	bionet_value_set_str(value, content);
	bionet_datapoint_set_timestamp(d, timestamp);
    }

    return 0;
} /* bionet_resource_set_str() */




// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
