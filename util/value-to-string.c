
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

char *bionet_value_to_str(const bionet_value_t *value)
{
    char * str;
    int r;
    /* sanity */
    if (NULL == value)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_to_str(): NULL value passed in");
	errno = EINVAL;
	return NULL;
    }

    if ((BIONET_RESOURCE_DATA_TYPE_STRING == value->resource->data_type)
	&& (NULL == value->content.string_v))
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_to_str(): NULL content in string value");
	errno = EINVAL;
	return NULL;	
    }

    str = malloc(BIONET_STR_MAX);
    if (NULL == str)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
	      "bionet_value_to_str(): Out of memory.");
	return NULL;
    }

    switch (value->resource->data_type)
    {
    case BIONET_RESOURCE_DATA_TYPE_BINARY:
	r = snprintf(str, BIONET_STR_MAX, "%d", value->content.binary_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_value_to_string(): value is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_UINT8:
	r = snprintf(str, BIONET_STR_MAX, "%hhu", value->content.uint8_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_value_to_string(): value is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_INT8:
	r = snprintf(str, BIONET_STR_MAX, "%hhd", value->content.int8_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_value_to_string(): value is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_UINT16:
	r = snprintf(str, BIONET_STR_MAX, "%hu", value->content.uint16_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_value_to_string(): value is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_INT16:
	r = snprintf(str, BIONET_STR_MAX, "%hd", value->content.int16_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_value_to_string(): value is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;
	
    case BIONET_RESOURCE_DATA_TYPE_UINT32:
	r = snprintf(str, BIONET_STR_MAX, "%u", value->content.uint32_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_value_to_string(): value is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_INT32:
	r = snprintf(str, BIONET_STR_MAX, "%d", value->content.int32_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_value_to_string(): value is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	r = snprintf(str, BIONET_STR_MAX, "%.7g", value->content.float_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_value_to_string(): value is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	r = snprintf(str, BIONET_STR_MAX, "%.14g", value->content.double_v);
	if (r >= BIONET_STR_MAX)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_value_to_string(): value is too big to fit in output string!");
	    free(str);
	    return NULL;
	}
	break;

    case BIONET_RESOURCE_DATA_TYPE_STRING:
	free(str);
	str = strdup(value->content.string_v);
	if (NULL == str)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		  "bionet_value_to_string(): Failed to copy string.");
	    free(str);
	    return NULL;
	}
	break;

    default:
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_value_to_string(): Unknown datatype.");
	free(str);
	return NULL;
    }
	
    return str;
} /* bionet_value_to_str() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
