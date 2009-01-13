
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet-util.h"
#include "internal.h"


int bionet_resource_get_binary(
    bionet_resource_t *resource, 
    int *content, 
    struct timeval *timestamp
) {
    bionet_datapoint_t *d;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_binary: passed in NULL Resource");
        return -1;
    }

    if (resource->data_type != BIONET_RESOURCE_DATA_TYPE_BINARY) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_binary: passed in Resource data type is not binary");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_binary: passed in Resource has no datapoints");
        return -1;
    }

    if (d->value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_binary: Resource datapoint has no value?!");
        return -1;
    }

    if (content != NULL) {
        *content = d->value->content.binary_v;
    }

    if (timestamp != NULL) {
        *timestamp = d->timestamp;
    }

    return 0;
}


int bionet_resource_get_uint8(
    bionet_resource_t *resource, 
    uint8_t *content, 
    struct timeval *timestamp
) {
    bionet_datapoint_t *d;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_uint8: passed in NULL Resource");
        return -1;
    }

    if (resource->data_type != BIONET_RESOURCE_DATA_TYPE_UINT8) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_uint8: passed in Resource data type is not uint8");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_uint8: passed in Resource has no datapoints");
        return -1;
    }

    if (d->value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_uint8: Resource datapoint has no value?!");
        return -1;
    }

    if (content != NULL) {
        *content = d->value->content.uint8_v;
    }

    if (timestamp != NULL) {
        *timestamp = d->timestamp;
    }

    return 0;
}


int bionet_resource_get_int8(
    bionet_resource_t *resource, 
    int8_t *content, 
    struct timeval *timestamp
) {
    bionet_datapoint_t *d;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_int8: passed in NULL Resource");
        return -1;
    }

    if (resource->data_type != BIONET_RESOURCE_DATA_TYPE_INT8) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_int8: passed in Resource data type is not int8");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_int8: passed in Resource has no datapoints");
        return -1;
    }

    if (d->value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_int8: Resource datapoint has no value?!");
        return -1;
    }

    if (content != NULL) {
        *content = d->value->content.int8_v;
    }

    if (timestamp != NULL) {
        *timestamp = d->timestamp;
    }

    return 0;
}


int bionet_resource_get_uint16(
    bionet_resource_t *resource, 
    uint16_t *content, 
    struct timeval *timestamp
) {
    bionet_datapoint_t *d;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_uint16: passed in NULL Resource");
        return -1;
    }

    if (resource->data_type != BIONET_RESOURCE_DATA_TYPE_UINT16) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_uint16: passed in Resource data type is not uint16");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_uint16: passed in Resource has no datapoints");
        return -1;
    }

    if (d->value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_uint16: Resource datapoint has no value?!");
        return -1;
    }

    if (content != NULL) {
        *content = d->value->content.uint16_v;
    }

    if (timestamp != NULL) {
        *timestamp = d->timestamp;
    }

    return 0;
}


int bionet_resource_get_int16(
    bionet_resource_t *resource, 
    int16_t *content, 
    struct timeval *timestamp
) {
    bionet_datapoint_t *d;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_int16: passed in NULL Resource");
        return -1;
    }

    if (resource->data_type != BIONET_RESOURCE_DATA_TYPE_INT16) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_int16: passed in Resource data type is not int16");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_int16: passed in Resource has no datapoints");
        return -1;
    }

    if (d->value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_int16: Resource datapoint has no value?!");
        return -1;
    }

    if (content != NULL) {
        *content = d->value->content.int16_v;
    }

    if (timestamp != NULL) {
        *timestamp = d->timestamp;
    }

    return 0;
}


int bionet_resource_get_uint32(
    bionet_resource_t *resource, 
    uint32_t *content, 
    struct timeval *timestamp
) {
    bionet_datapoint_t *d;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_uint32: passed in NULL Resource");
        return -1;
    }

    if (resource->data_type != BIONET_RESOURCE_DATA_TYPE_UINT32) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_uint32: passed in Resource data type is not uint32");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_uint32: passed in Resource has no datapoints");
        return -1;
    }

    if (d->value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_uint32: Resource datapoint has no value?!");
        return -1;
    }

    if (content != NULL) {
        *content = d->value->content.uint32_v;
    }

    if (timestamp != NULL) {
        *timestamp = d->timestamp;
    }

    return 0;
}


int bionet_resource_get_int32(
    bionet_resource_t *resource, 
    int32_t *content, 
    struct timeval *timestamp
) {
    bionet_datapoint_t *d;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_int32: passed in NULL Resource");
        return -1;
    }

    if (resource->data_type != BIONET_RESOURCE_DATA_TYPE_INT32) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_int32: passed in Resource data type is not int32");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_int32: passed in Resource has no datapoints");
        return -1;
    }

    if (d->value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_int32: Resource datapoint has no value?!");
        return -1;
    }

    if (content != NULL) {
        *content = d->value->content.int32_v;
    }

    if (timestamp != NULL) {
        *timestamp = d->timestamp;
    }

    return 0;
}


int bionet_resource_get_float(
    bionet_resource_t *resource, 
    float *content, 
    struct timeval *timestamp
) {
    bionet_datapoint_t *d;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_float: passed in NULL Resource");
        return -1;
    }

    if (resource->data_type != BIONET_RESOURCE_DATA_TYPE_FLOAT) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_float: passed in Resource data type is not float");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_float: passed in Resource has no datapoints");
        return -1;
    }

    if (d->value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_float: Resource datapoint has no value?!");
        return -1;
    }

    if (content != NULL) {
        *content = d->value->content.float_v;
    }

    if (timestamp != NULL) {
        *timestamp = d->timestamp;
    }

    return 0;
}


int bionet_resource_get_double(
    bionet_resource_t *resource, 
    double *content, 
    struct timeval *timestamp
) {
    bionet_datapoint_t *d;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_double: passed in NULL Resource");
        return -1;
    }

    if (resource->data_type != BIONET_RESOURCE_DATA_TYPE_DOUBLE) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_double: passed in Resource data type is not double");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_double: passed in Resource has no datapoints");
        return -1;
    }

    if (d->value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_double: Resource datapoint has no value?!");
        return -1;
    }

    if (content != NULL) {
        *content = d->value->content.double_v;
    }

    if (timestamp != NULL) {
        *timestamp = d->timestamp;
    }

    return 0;
}


int bionet_resource_get_str(
    bionet_resource_t *resource, 
    char * *content, 
    struct timeval *timestamp
) {
    bionet_datapoint_t *d;

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_str: passed in NULL Resource");
        return -1;
    }

    if (resource->data_type != BIONET_RESOURCE_DATA_TYPE_STRING) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_str: passed in Resource data type is not string");
        return -1;
    }

    d = bionet_resource_get_datapoint_by_index(resource, 0);
    if (d == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_str: passed in Resource has no datapoints");
        return -1;
    }

    if (d->value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_str: Resource datapoint has no value?!");
        return -1;
    }

    if (content != NULL) {
        *content = strdup(d->value->content.string_v);
        if (*content == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_get_str: out of memory!");
            return -1;
        }
    }

    if (timestamp != NULL) {
        *timestamp = d->timestamp;
    }

    return 0;
}

