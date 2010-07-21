
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "bionet-util.h"
#include "internal.h"

#define BIONET_STR_MAX 512

char *bionet_delta_to_str(const bionet_delta_t *delta,
			  bionet_resource_data_type_t data_type)
{
    char * str;
    int r;
    /* sanity */
    if (NULL == delta)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_delta_to_str(): NULL delta passed in");
	errno = EINVAL;
	return NULL;
    }

    str = malloc(BIONET_STR_MAX);
    if (NULL == str)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
	      "bionet_delta_to_str(): Out of memory.");
	return NULL;
    }

    switch (data_type)
    {
    case BIONET_RESOURCE_DATA_TYPE_BINARY:
	r = snprintf(str, BIONET_STR_MAX, "%d", delta->content.binary_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_delta_to_string(): delta is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_UINT8:
	r = snprintf(str, BIONET_STR_MAX, "%hhu", delta->content.uint8_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_delta_to_string(): delta is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_INT8:
	r = snprintf(str, BIONET_STR_MAX, "%hhd", delta->content.int8_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_delta_to_string(): delta is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_UINT16:
	r = snprintf(str, BIONET_STR_MAX, "%hu", delta->content.uint16_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_delta_to_string(): delta is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_INT16:
	r = snprintf(str, BIONET_STR_MAX, "%hd", delta->content.int16_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_delta_to_string(): delta is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;
	
    case BIONET_RESOURCE_DATA_TYPE_UINT32:
	r = snprintf(str, BIONET_STR_MAX, "%u", delta->content.uint32_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_delta_to_string(): delta is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_INT32:
	r = snprintf(str, BIONET_STR_MAX, "%d", delta->content.int32_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_delta_to_string(): delta is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	r = snprintf(str, BIONET_STR_MAX, "%.7g", delta->content.float_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_delta_to_string(): delta is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	r = snprintf(str, BIONET_STR_MAX, "%.14g", delta->content.double_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_delta_to_string(): delta is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_STRING:
	r = snprintf(str, BIONET_STR_MAX, "%d", delta->content.string_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_delta_to_string(): delta is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    default:
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_delta_to_string(): Unknown datatype.");
	free(str);
	return NULL;
    }
	
    return str;
} /* bionet_delta_to_str() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
