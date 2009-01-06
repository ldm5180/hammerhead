
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>

#include "bionet-util.h"
#include "internal.h"


static int bionet_value_get_internal(bionet_value_t *value,
				     void * content, 
				     bionet_resource_data_type_t datatype);


int bionet_value_get_binary(bionet_value_t *value,
			    int * content)
{
    return bionet_value_get_internal(value, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_BINARY);
} /* bionet_value_get_binary() */


int bionet_value_get_uint8(bionet_value_t *value,
			   uint8_t * content)
{
    return bionet_value_get_internal(value, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_UINT8);
} /* bionet_value_get_uint8() */


int bionet_value_get_int8(bionet_value_t *value,
			  int8_t * content)
{
    return bionet_value_get_internal(value, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_INT8);
} /* bionet_value_get_int8() */


int bionet_value_get_uint16(bionet_value_t *value,
			    uint16_t * content)
{
    return bionet_value_get_internal(value, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_UINT16);
} /* bionet_value_get_uint16() */


int bionet_value_get_int16(bionet_value_t *value,
			   int16_t * content)
{
    return bionet_value_get_internal(value, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_INT16);
} /* bionet_value_get_int16() */


int bionet_value_get_uint32(bionet_value_t *value,
			    uint32_t * content)
{
    return bionet_value_get_internal(value, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_UINT32);
} /* bionet_value_get_uint32() */


int bionet_value_get_int32(bionet_value_t *value,
			   int32_t * content)
{
    return bionet_value_get_internal(value, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_INT32);
} /* bionet_value_get_int32() */


int bionet_value_get_float(bionet_value_t *value,
			   float * content)
{
    return bionet_value_get_internal(value, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_FLOAT);
} /* bionet_value_get_float() */


int bionet_value_get_double(bionet_value_t *value,
			    double * content)
{
    return bionet_value_get_internal(value, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_DOUBLE);
} /* bionet_value_get_double() */


int bionet_value_get_str(bionet_value_t *value,
			 char ** content)
{
    return bionet_value_get_internal(value, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_STRING);
} /* bionet_value_get_str() */


static int bionet_value_get_internal(bionet_value_t *value,
				     void * content, 
				     bionet_resource_data_type_t datatype)
{
    if (NULL == value)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_get_*(): NULL value passed in");
	errno = EINVAL;
	return -1;
    }

    if (NULL == content)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_get_*(): NULL content passed in");
	errno = EINVAL;
	return -1;
    }

    if ((BIONET_RESOURCE_DATA_TYPE_MIN > datatype) 
	|| (BIONET_RESOURCE_DATA_TYPE_MAX < datatype))
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_get_*(): Invalid datatype passed in");
	errno = EINVAL;
	return -1;
    }

    if (value->resource->data_type != datatype)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_get_*(): resource/datatype mismatch");
	errno = EINVAL;
	return -1;
    }

    /* get content */
    switch (datatype)
    {
    case BIONET_RESOURCE_DATA_TYPE_BINARY:
    {
	int *pContent = (int *)content;
	*pContent = value->content.binary_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT8:
    {
	uint8_t * pContent = (uint8_t *)content;
	*pContent = value->content.uint8_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT8:
    {
	int8_t * pContent = (int8_t *)content;
	*pContent = value->content.int8_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT16:
    {
	uint16_t * pContent = (uint16_t *)content;
	*pContent = value->content.uint16_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT16:
    {
	int16_t * pContent = (int16_t *)content;
	*pContent = value->content.int16_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT32:
    {
	uint32_t * pContent = (uint32_t *)content;
	*pContent = value->content.uint32_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT32:
    {
	int32_t * pContent = (int32_t *)content;
	*pContent = value->content.int32_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_FLOAT:
    {
	float * pContent = (float *)content;
	*pContent = value->content.float_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
    {
	double * pContent = (double *)content;
	*pContent = value->content.double_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_STRING:
    {
	char ** pContent = (char **)content;
	*pContent = value->content.string_v;
	break;	
    }
    default:
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_get_*(): Invalid datatype");
	errno = EINVAL;
	free(value);
	return -1;
    }

    return 0;
} /* bionet_value_get_internal() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
