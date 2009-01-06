
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include "bionet-util.h"
#include "internal.h"


static int bionet_value_set_internal(bionet_value_t *value,
				     const void *content,
				     bionet_resource_data_type_t datatype);


int bionet_value_set_binary(bionet_value_t *value,
			    int content)
{
    return bionet_value_set_internal(value, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_BINARY);
} /* bionet_value_set_binary() */


int bionet_value_set_uint8(bionet_value_t *value,
			   uint8_t content)
{
    return bionet_value_set_internal(value, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_UINT8);
} /* bionet_value_set_uint8() */


int bionet_value_set_int8(bionet_value_t *value,
			  int8_t content)
{
    return bionet_value_set_internal(value, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_INT8);
} /* bionet_value_set_int8() */


int bionet_value_set_uint16(bionet_value_t *value,
			    uint16_t content)
{
    return bionet_value_set_internal(value, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_UINT16);
} /* bionet_value_set_uint16() */


int bionet_value_set_int16(bionet_value_t *value,
			   int16_t content)
{
    return bionet_value_set_internal(value, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_INT16);
} /* bionet_value_set_int16() */


int bionet_value_set_uint32(bionet_value_t *value,
			    uint32_t content)
{
    return bionet_value_set_internal(value, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_UINT32);
} /* bionet_value_set_uint32() */


int bionet_value_set_int32(bionet_value_t *value,
			   int32_t content)
{
    return bionet_value_set_internal(value, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_INT32);
} /* bionet_value_set_int32() */


int bionet_value_set_float(bionet_value_t *value,
			   float content)
{
    return bionet_value_set_internal(value, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_FLOAT);
} /* bionet_value_set_float() */


int bionet_value_set_double(bionet_value_t *value,
			    double content)
{
    return bionet_value_set_internal(value, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_DOUBLE);
} /* bionet_value_set_double() */


int bionet_value_set_str(bionet_value_t *value,
			 const char * content)
{
    return bionet_value_set_internal(value, 
				     content, 
				     BIONET_RESOURCE_DATA_TYPE_STRING);
    /* bionet_value_set_str() */
}

static int bionet_value_set_internal(bionet_value_t *value,
				     const void *content,
				     bionet_resource_data_type_t datatype)
{
    bionet_datapoint_t *dp = NULL;

    /* sanity */
    if (NULL == value)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_set_*(): NULL value passed in");
	errno = EINVAL;
	return -1;	
    }

    if (NULL == content)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_set_*(): NULL content passed in");
	errno = EINVAL;
	return -1;
    }

    if ((BIONET_RESOURCE_DATA_TYPE_MIN > datatype) 
	|| (BIONET_RESOURCE_DATA_TYPE_MAX < datatype))
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_set_*(): Invalid datatype passed in");
	errno = EINVAL;
	return -1;
    }

    if ((NULL != value->resource) && (value->resource->data_type != datatype))
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_set_*(): resource/datatype mismatch");
	errno = EINVAL;
	return -1;
    }

    /* set content */
    switch (datatype)
    {
    case BIONET_RESOURCE_DATA_TYPE_BINARY:
    {
	int * pContent = (int *)content;
	value->content.binary_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT8:
    {
	uint8_t * pContent = (uint8_t *)content;
	value->content.uint8_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT8:
    {
	int8_t * pContent = (int8_t *)content;
	value->content.int8_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT16:
    {
	uint16_t * pContent = (uint16_t *)content;
	value->content.uint16_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT16:
    {
	int16_t * pContent = (int16_t *)content;
	value->content.int16_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT32:
    {
	uint32_t * pContent = (uint32_t *)content;
	value->content.uint32_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT32:
    {
	int32_t * pContent = (int32_t *)content;
	value->content.int32_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_FLOAT:
    {
	float * pContent = (float *)content;
	value->content.float_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
    {
	double * pContent = (double *)content;
	value->content.double_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_STRING:
    {
	if (value->content.string_v)
	{
	    /* free the previous content */
	    free(value->content.string_v); 
	}
	value->content.string_v = (char *)content;
	break;	
    }
    default:
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_get_*(): Invalid datatype");
	errno = EINVAL;
	free(value);
	return -1;
    }

    dp = bionet_resource_get_datapoint_by_index(value->resource, 0);
    if (dp)
    {
	dp->dirty = 1;
    }

    return 0;
} /* bionet_value_set_internal() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
