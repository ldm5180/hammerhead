
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "bionet.h"

bionet_value_t *str_to_value(bionet_resource_t *resource, bionet_resource_data_type_t data_type, char *str) {
    bionet_value_t *value;

    /* the value string is enclosed in ', so we have to strip them */
    str++;
    str[strlen(str)-1] = '\0';

    switch (data_type) {
        case BIONET_RESOURCE_DATA_TYPE_BINARY:
            value = bionet_value_new_binary(resource, atoi(str));
            break;

        case BIONET_RESOURCE_DATA_TYPE_UINT8:
            value = bionet_value_new_uint8(resource, atoi(str));
            break;

        case BIONET_RESOURCE_DATA_TYPE_UINT16:
            value = bionet_value_new_uint16(resource, atoi(str));
            break;

        case BIONET_RESOURCE_DATA_TYPE_UINT32:
            value = bionet_value_new_uint32(resource, atoi(str));
            break;

        case BIONET_RESOURCE_DATA_TYPE_INT8:
            value = bionet_value_new_int8(resource, atoi(str));
            break;

        case BIONET_RESOURCE_DATA_TYPE_INT16:
            value = bionet_value_new_int16(resource, atoi(str));
            break;

        case BIONET_RESOURCE_DATA_TYPE_INT32:
            value = bionet_value_new_int32(resource, atoi(str));
            break;

        case BIONET_RESOURCE_DATA_TYPE_FLOAT:
            value = bionet_value_new_float(resource, atof(str));
            break;

        case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
            value = bionet_value_new_double(resource, strtod(str, NULL));
            break;

        case BIONET_RESOURCE_DATA_TYPE_STRING: {
            char *allocated_string;
            allocated_string = strdup(str);
            value = bionet_value_new_str(resource, allocated_string);
            break;
        }

        default: 
            g_log("", G_LOG_LEVEL_WARNING, "unable to translate create bionet_value_t* from '%s'\n", str);
            value = NULL;
            break;
    }

    str--;
    str[strlen(str)-1] = '\'';
    free(str);

    return value;
}
