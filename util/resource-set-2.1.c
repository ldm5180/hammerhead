
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet-util-2.1.h"
#include "internal.h"



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

	bionet_value_set_binary(value, content);

	bionet_datapoint_set_value(d, value);
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

	bionet_value_set_uint8(value, content);

	bionet_datapoint_set_value(d, value);
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

	bionet_value_set_int8(value, content);

	bionet_datapoint_set_value(d, value);
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

	bionet_value_set_uint16(value, content);

	bionet_datapoint_set_value(d, value);
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

	bionet_value_set_int16(value, content);

	bionet_datapoint_set_value(d, value);
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

	bionet_value_set_uint32(value, content);

	bionet_datapoint_set_value(d, value);
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

	bionet_value_set_int32(value, content);

	bionet_datapoint_set_value(d, value);
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

	bionet_value_set_float(value, content);

	bionet_datapoint_set_value(d, value);
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

	bionet_value_set_double(value, content);

	bionet_datapoint_set_value(d, value);
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

	bionet_datapoint_set_value(d, value);
	bionet_datapoint_set_timestamp(d, timestamp);
    }

    return 0;
} /* bionet_resource_set_str() */




// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
