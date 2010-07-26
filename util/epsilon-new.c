
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "bionet-util.h"
#include "internal.h"


/* function declarations */
static bionet_epsilon_t * bionet_epsilon_new_internal(const void *content,
						  bionet_resource_data_type_t datatype);


/* function implementations */
bionet_epsilon_t *bionet_epsilon_new_binary(int content)
{
    return bionet_epsilon_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_BINARY);
} /* bionet_epsilon_new_binary() */


bionet_epsilon_t *bionet_epsilon_new_uint8(uint8_t content)
{
    return bionet_epsilon_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_UINT8);
} /* bionet_epsilon_new_uint8() */


bionet_epsilon_t *bionet_epsilon_new_int8(int8_t content)
{
    return bionet_epsilon_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_INT8);
} /* bionet_epsilon_new_int8() */


bionet_epsilon_t *bionet_epsilon_new_uint16(uint16_t content)
{
    return bionet_epsilon_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_UINT16);
} /* bionet_epsilon_new_uint16() */


bionet_epsilon_t *bionet_epsilon_new_int16(int16_t content)
{
    return bionet_epsilon_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_INT16);
} /* bionet_epsilon_new_int16() */


bionet_epsilon_t *bionet_epsilon_new_uint32(uint32_t content)
{
    return bionet_epsilon_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_UINT32);
} /* bionet_epsilon_new_uint32() */


bionet_epsilon_t *bionet_epsilon_new_int32(int32_t content)
{
    return bionet_epsilon_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_INT32);
} /* bionet_epsilon_new_int32() */


bionet_epsilon_t *bionet_epsilon_new_float(float content)
{
    return bionet_epsilon_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_FLOAT);
} /* bionet_epsilon_new_float() */


bionet_epsilon_t *bionet_epsilon_new_double(double content)
{
    return bionet_epsilon_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_DOUBLE);
} /* bionet_epsilon_new_double() */


bionet_epsilon_t *bionet_epsilon_new_str(int content)
{
    return bionet_epsilon_new_internal(&content, 
				     BIONET_RESOURCE_DATA_TYPE_STRING);
} /* bionet_epsilon_new_string() */


static bionet_epsilon_t * bionet_epsilon_new_internal(const void *content,
						  bionet_resource_data_type_t datatype)
{
    bionet_epsilon_t * epsilon;

    if (NULL == content)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_epsilon_new_*(): NULL content passed in");
	errno = EINVAL;
	return NULL;
    }

    /* allocation */
    epsilon = calloc(1, sizeof(bionet_epsilon_t));
    if (NULL == epsilon)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
	      "bionet_epsilon_new_binary(): out of memory!");
        return NULL;
    }

    /* set content */
    switch (datatype)
    {
    case BIONET_RESOURCE_DATA_TYPE_BINARY:
    {
	int * pContent = (int *)content;
	epsilon->content.binary_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT8:
    {
	uint8_t * pContent = (uint8_t *)content;
	epsilon->content.uint8_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT8:
    {
	int8_t * pContent = (int8_t *)content;
	epsilon->content.int8_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT16:
    {
	uint16_t * pContent = (uint16_t *)content;
	epsilon->content.uint16_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT16:
    {
	int16_t * pContent = (int16_t *)content;
	epsilon->content.int16_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT32:
    {
	uint32_t * pContent = (uint32_t *)content;
	epsilon->content.uint32_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT32:
    {
	int32_t * pContent = (int32_t *)content;
	epsilon->content.int32_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_FLOAT:
    {
	float * pContent = (float *)content;
	epsilon->content.float_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
    {
	double * pContent = (double *)content;
	epsilon->content.double_v = *pContent;
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_STRING:
    {
	int * pContent = (int *)content;
	epsilon->content.string_v = *pContent;
	break;	
    }
    default:
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_epsilon_new_*(): Invalid datatype %d", datatype);
	errno = EINVAL;
	free(epsilon);
	return NULL;
    }

    return epsilon;
} /* bionet_epsilon_new_internal() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
