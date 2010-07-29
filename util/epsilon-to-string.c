
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

char *bionet_epsilon_to_str(const bionet_epsilon_t *epsilon,
			  bionet_resource_data_type_t data_type)
{
    char * str;
    int r;
    /* sanity */
    if (NULL == epsilon)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_epsilon_to_str(): NULL epsilon passed in");
	errno = EINVAL;
	return NULL;
    }

    str = malloc(BIONET_STR_MAX);
    if (NULL == str)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
	      "bionet_epsilon_to_str(): Out of memory.");
	return NULL;
    }

    switch (data_type)
    {
    case BIONET_RESOURCE_DATA_TYPE_BINARY:
	r = snprintf(str, BIONET_STR_MAX, "%d", epsilon->content.binary_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_epsilon_to_string(): epsilon is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_UINT8:
	r = snprintf(str, BIONET_STR_MAX, "%hhu", epsilon->content.uint8_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_epsilon_to_string(): epsilon is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_INT8:
	r = snprintf(str, BIONET_STR_MAX, "%hhd", epsilon->content.int8_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_epsilon_to_string(): epsilon is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_UINT16:
	r = snprintf(str, BIONET_STR_MAX, "%hu", epsilon->content.uint16_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_epsilon_to_string(): epsilon is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_INT16:
	r = snprintf(str, BIONET_STR_MAX, "%hd", epsilon->content.int16_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_epsilon_to_string(): epsilon is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;
	
    case BIONET_RESOURCE_DATA_TYPE_UINT32:
	r = snprintf(str, BIONET_STR_MAX, "%u", epsilon->content.uint32_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_epsilon_to_string(): epsilon is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_INT32:
	r = snprintf(str, BIONET_STR_MAX, "%d", epsilon->content.int32_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_epsilon_to_string(): epsilon is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	r = snprintf(str, BIONET_STR_MAX, "%.7g", epsilon->content.float_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_epsilon_to_string(): epsilon is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	r = snprintf(str, BIONET_STR_MAX, "%.14g", epsilon->content.double_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_epsilon_to_string(): epsilon is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_STRING:
	r = snprintf(str, BIONET_STR_MAX, "%d", epsilon->content.string_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_epsilon_to_string(): epsilon is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    default:
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_epsilon_to_string(): Unknown datatype.");
	free(str);
	return NULL;
    }
	
    return str;
} /* bionet_epsilon_to_str() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
