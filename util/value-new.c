
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "bionet-util.h"
#include "internal.h"


/* function declarations */
static bionet_value_t * bionet_value_new_internal(const bionet_resource_t *resource,
						  const void *content,
						  bionet_resource_data_type_t datatype);


/* function implementations */
bionet_value_t *bionet_value_new_binary(const bionet_resource_t *resource,
					int content)
{
    return bionet_value_new_internal(resource, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_BINARY);
} /* bionet_value_new_binary() */


bionet_value_t *bionet_value_new_uint8(const bionet_resource_t *resource,
				       uint8_t content)
{
    return bionet_value_new_internal(resource, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_UINT8);
} /* bionet_value_new_uint8() */


bionet_value_t *bionet_value_new_int8(const bionet_resource_t *resource,
				      int8_t content)
{
    return bionet_value_new_internal(resource, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_INT8);
} /* bionet_value_new_int8() */


bionet_value_t *bionet_value_new_uint16(const bionet_resource_t *resource,
					uint16_t content)
{
    return bionet_value_new_internal(resource, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_UINT16);
} /* bionet_value_new_uint16() */


bionet_value_t *bionet_value_new_int16(const bionet_resource_t *resource,
				       int16_t content)
{
    return bionet_value_new_internal(resource, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_INT16);
} /* bionet_value_new_int16() */


bionet_value_t *bionet_value_new_uint32(const bionet_resource_t *resource,
					uint32_t content)
{
    return bionet_value_new_internal(resource, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_UINT32);
} /* bionet_value_new_uint32() */


bionet_value_t *bionet_value_new_int32(const bionet_resource_t *resource,
				       int32_t content)
{
    return bionet_value_new_internal(resource, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_INT32);
} /* bionet_value_new_int32() */


bionet_value_t *bionet_value_new_float(const bionet_resource_t *resource,
				       float content)
{
    return bionet_value_new_internal(resource, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_FLOAT);
} /* bionet_value_new_float() */


bionet_value_t *bionet_value_new_double(const bionet_resource_t *resource,
				        double content)
{
    return bionet_value_new_internal(resource, 
				     &content, 
				     BIONET_RESOURCE_DATA_TYPE_DOUBLE);
} /* bionet_value_new_double() */


bionet_value_t *bionet_value_new_str(const bionet_resource_t *resource,
				     const char * content)
{
    return bionet_value_new_internal(resource, 
				     content, 
				     BIONET_RESOURCE_DATA_TYPE_STRING);
} /* bionet_value_new_string() */


bionet_value_t * bionet_value_dup(const bionet_resource_t *resource, const bionet_value_t *value)
{

    switch (resource->data_type) {
	
    case BIONET_RESOURCE_DATA_TYPE_BINARY:
	return bionet_value_new_internal(resource, &value->content.binary_v, BIONET_RESOURCE_DATA_TYPE_BINARY);

    case BIONET_RESOURCE_DATA_TYPE_UINT8:
	return bionet_value_new_internal(resource, &value->content.uint8_v, BIONET_RESOURCE_DATA_TYPE_UINT8);

    case BIONET_RESOURCE_DATA_TYPE_INT8:
	return bionet_value_new_internal(resource, &value->content.int8_v, BIONET_RESOURCE_DATA_TYPE_INT8);

    case BIONET_RESOURCE_DATA_TYPE_UINT16:
	return bionet_value_new_internal(resource, &value->content.uint16_v, BIONET_RESOURCE_DATA_TYPE_UINT16);

    case BIONET_RESOURCE_DATA_TYPE_INT16:
	return bionet_value_new_internal(resource, &value->content.int16_v, BIONET_RESOURCE_DATA_TYPE_INT16);

    case BIONET_RESOURCE_DATA_TYPE_UINT32:
	return bionet_value_new_internal(resource, &value->content.uint32_v, BIONET_RESOURCE_DATA_TYPE_UINT32);

    case BIONET_RESOURCE_DATA_TYPE_INT32:
	return bionet_value_new_internal(resource, &value->content.int32_v, BIONET_RESOURCE_DATA_TYPE_INT32);

    case BIONET_RESOURCE_DATA_TYPE_FLOAT:
	return bionet_value_new_internal(resource, &value->content.float_v, BIONET_RESOURCE_DATA_TYPE_FLOAT);

    case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
	return bionet_value_new_internal(resource, &value->content.double_v, BIONET_RESOURCE_DATA_TYPE_DOUBLE);

    case BIONET_RESOURCE_DATA_TYPE_STRING:
    {
	return bionet_value_new_internal(resource, value->content.string_v, BIONET_RESOURCE_DATA_TYPE_STRING);
    }

    default:
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Invalid data type %d", resource->data_type);
	break;
    }

    return NULL;
} /* bionet_value_dup() */


static bionet_value_t * bionet_value_new_internal(const bionet_resource_t *resource,
						  const void *content,
						  bionet_resource_data_type_t datatype)
{
    bionet_value_t * value;

    /* sanity */
    if (NULL == resource)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_new_*(): NULL resource passed in");
	errno = EINVAL;
	return NULL;
    }

    if (NULL == content)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_new_*(): NULL content passed in");
	errno = EINVAL;
	return NULL;
    }

    if (resource->data_type != datatype)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_new_*(): resource/datatype mismatch");
	errno = EINVAL;
	return NULL;
    }

    /* allocation */
    value = calloc(1, sizeof(bionet_value_t));
    if (NULL == value)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
	      "bionet_value_new_binary(): out of memory!");
        return NULL;
    }

    /* set resource */
    value->resource = (bionet_resource_t *)resource;
    
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
	value->content.string_v = strdup((char *)content);
        if (value->content.string_v == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_value_new_*(): out of memory!");
            free(value);
            errno = ENOMEM;
            return NULL;
	}
	break;	
    }
    default:
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_value_new_*(): Invalid datatype %d", datatype);
	errno = EINVAL;
	free(value);
	return NULL;
    }

    return value;
} /* bionet_value_new_internal() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
