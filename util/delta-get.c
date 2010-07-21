
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>

#include "bionet-util.h"
#include "internal.h"


static int bionet_delta_get_internal(const bionet_delta_t *delta,
				     void * content, 
				     bionet_resource_data_type_t datatype);


int bionet_delta_get_binary(const bionet_delta_t *delta,
			    int * content)
{
    return bionet_delta_get_internal(delta, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_BINARY);
} /* bionet_delta_get_binary() */


int bionet_delta_get_uint8(const bionet_delta_t *delta,
			   uint8_t * content)
{
    return bionet_delta_get_internal(delta, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_UINT8);
} /* bionet_delta_get_uint8() */


int bionet_delta_get_int8(const bionet_delta_t *delta,
			  int8_t * content)
{
    return bionet_delta_get_internal(delta, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_INT8);
} /* bionet_delta_get_int8() */


int bionet_delta_get_uint16(const bionet_delta_t *delta,
			    uint16_t * content)
{
    return bionet_delta_get_internal(delta, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_UINT16);
} /* bionet_delta_get_uint16() */


int bionet_delta_get_int16(const bionet_delta_t *delta,
			   int16_t * content)
{
    return bionet_delta_get_internal(delta, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_INT16);
} /* bionet_delta_get_int16() */


int bionet_delta_get_uint32(const bionet_delta_t *delta,
			    uint32_t * content)
{
    return bionet_delta_get_internal(delta, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_UINT32);
} /* bionet_delta_get_uint32() */


int bionet_delta_get_int32(const bionet_delta_t *delta,
			   int32_t * content)
{
    return bionet_delta_get_internal(delta, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_INT32);
} /* bionet_delta_get_int32() */


int bionet_delta_get_float(const bionet_delta_t *delta,
			   float * content)
{
    return bionet_delta_get_internal(delta, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_FLOAT);
} /* bionet_delta_get_float() */


int bionet_delta_get_double(const bionet_delta_t *delta,
			    double * content)
{
    return bionet_delta_get_internal(delta, 
				     content,
				     BIONET_RESOURCE_DATA_TYPE_DOUBLE);
} /* bionet_delta_get_double() */


int bionet_delta_get_str(const bionet_delta_t *delta,
			 int * content)
{
    return bionet_delta_get_internal(delta, 
				     &content,
				     BIONET_RESOURCE_DATA_TYPE_STRING);
} /* bionet_delta_get_str() */


static int bionet_delta_get_internal(const bionet_delta_t *delta,
				     void * content, 
				     bionet_resource_data_type_t datatype)
{
    if (NULL == delta)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_delta_get_*(): NULL delta passed in");
	errno = EINVAL;
	return -1;
    }

    if (NULL == content)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_delta_get_*(): NULL content passed in");
	errno = EINVAL;
	return -1;
    }

    /* get content */
    switch (datatype)
    {
    case BIONET_RESOURCE_DATA_TYPE_BINARY:
    {
	int *pContent = (int *)content;
	*pContent = delta->content.binary_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT8:
    {
	uint8_t * pContent = (uint8_t *)content;
	*pContent = delta->content.uint8_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT8:
    {
	int8_t * pContent = (int8_t *)content;
	*pContent = delta->content.int8_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT16:
    {
	uint16_t * pContent = (uint16_t *)content;
	*pContent = delta->content.uint16_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT16:
    {
	int16_t * pContent = (int16_t *)content;
	*pContent = delta->content.int16_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT32:
    {
	uint32_t * pContent = (uint32_t *)content;
	*pContent = delta->content.uint32_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT32:
    {
	int32_t * pContent = (int32_t *)content;
	*pContent = delta->content.int32_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_FLOAT:
    {
	float * pContent = (float *)content;
	*pContent = delta->content.float_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
    {
	double * pContent = (double *)content;
	*pContent = delta->content.double_v;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_STRING:
    {
	int *pContent = (int *)content;
	*pContent = delta->content.string_v;
	break;	
    }
    default:
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_delta_get_*(): Invalid datatype %d", datatype);
	errno = EINVAL;
	return -1;
    }

    return 0;
} /* bionet_value_get_internal() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
