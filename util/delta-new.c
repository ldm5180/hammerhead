
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "bionet-util.h"
#include "internal.h"


/* function declarations */
static bionet_delta_t * bionet_delta_new_internal(const void *content,
						  bionet_resource_data_type_t datatype);


/* function implementations */
bionet_delta_t *bionet_delta_new_binary(int content)
{
    return bionet_delta_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_BINARY);
} /* bionet_delta_new_binary() */


bionet_delta_t *bionet_delta_new_uint8(uint8_t content)
{
    return bionet_delta_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_UINT8);
} /* bionet_delta_new_uint8() */


bionet_delta_t *bionet_delta_new_int8(int8_t content)
{
    return bionet_delta_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_INT8);
} /* bionet_delta_new_int8() */


bionet_delta_t *bionet_delta_new_uint16(uint16_t content)
{
    return bionet_delta_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_UINT16);
} /* bionet_delta_new_uint16() */


bionet_delta_t *bionet_delta_new_int16(int16_t content)
{
    return bionet_delta_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_INT16);
} /* bionet_delta_new_int16() */


bionet_delta_t *bionet_delta_new_uint32(uint32_t content)
{
    return bionet_delta_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_UINT32);
} /* bionet_delta_new_uint32() */


bionet_delta_t *bionet_delta_new_int32(int32_t content)
{
    return bionet_delta_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_INT32);
} /* bionet_delta_new_int32() */


bionet_delta_t *bionet_delta_new_float(float content)
{
    return bionet_delta_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_FLOAT);
} /* bionet_delta_new_float() */


bionet_delta_t *bionet_delta_new_double(double content)
{
    return bionet_delta_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_DOUBLE);
} /* bionet_delta_new_double() */


bionet_delta_t *bionet_delta_new_str(int content)
{
    return bionet_delta_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_STRING);
} /* bionet_delta_new_string() */


static bionet_delta_t * bionet_delta_new_internal(const void *content,
						  bionet_resource_data_type_t datatype)
{
    bionet_delta_t * delta;

    if (NULL == content)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_delta_new_*(): NULL content passed in");
	errno = EINVAL;
	return NULL;
    }

    /* allocation */
    delta = calloc(1, sizeof(bionet_delta_t));
    if (NULL == delta)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
	      "bionet_delta_new_binary(): out of memory!");
        return NULL;
    }

    /* set content */
    switch (datatype)
    {
    case BIONET_RESOURCE_DATA_TYPE_BINARY:
    {
	int * pContent = (int *)content;
	delta->content.binary_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT8:
    {
	uint8_t * pContent = (uint8_t *)content;
	delta->content.uint8_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT8:
    {
	int8_t * pContent = (int8_t *)content;
	delta->content.int8_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT16:
    {
	uint16_t * pContent = (uint16_t *)content;
	delta->content.uint16_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT16:
    {
	int16_t * pContent = (int16_t *)content;
	delta->content.int16_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT32:
    {
	uint32_t * pContent = (uint32_t *)content;
	delta->content.uint32_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT32:
    {
	int32_t * pContent = (int32_t *)content;
	delta->content.int32_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_FLOAT:
    {
	float * pContent = (float *)content;
	delta->content.float_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
    {
	double * pContent = (double *)content;
	delta->content.double_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_STRING:
    {
	int * pContent = (int *)content;
	delta->content.string_v = *pContent;
	break;	
    }
    default:
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_delta_new_*(): Invalid datatype %d", datatype);
	errno = EINVAL;
	free(delta);
	return NULL;
    }

    return delta;
} /* bionet_delta_new_internal() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
