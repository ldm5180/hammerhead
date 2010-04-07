
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include "internal.h"
#include "bionet-util.h"


const char *bionet_resource_data_type_to_string(bionet_resource_data_type_t data_type) {
    static char *resource_data_type_to_string_array[] = {
        "(invalid)",
        "Binary",
        "UInt8",
        "Int8",
        "UInt16",
        "Int16",
        "UInt32",
        "Int32",
        "Float",
        "Double",
        "String"
    };

    if (
        (data_type < BIONET_RESOURCE_DATA_TYPE_MIN) ||
        (data_type > BIONET_RESOURCE_DATA_TYPE_MAX)
    ) {
        return resource_data_type_to_string_array[0];  // invalid
    }

    return resource_data_type_to_string_array[data_type+1];
}


bionet_resource_data_type_t bionet_resource_data_type_from_string(const char *data_type_string) {
    if (data_type_string == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_data_type_from_string(): NULL data_type_string passed in!");
        return BIONET_RESOURCE_DATA_TYPE_INVALID;
    }

    if (strcasecmp(data_type_string, "binary") == 0) {
        return BIONET_RESOURCE_DATA_TYPE_BINARY;

    } else if (strcasecmp(data_type_string, "uint8") == 0) {
        return BIONET_RESOURCE_DATA_TYPE_UINT8;

    } else if (strcasecmp(data_type_string, "int8") == 0) {
        return BIONET_RESOURCE_DATA_TYPE_INT8;

    } else if (strcasecmp(data_type_string, "uint16") == 0) {
        return BIONET_RESOURCE_DATA_TYPE_UINT16;

    } else if (strcasecmp(data_type_string, "int16") == 0) {
        return BIONET_RESOURCE_DATA_TYPE_INT16;

    } else if (strcasecmp(data_type_string, "uint32") == 0) {
        return BIONET_RESOURCE_DATA_TYPE_UINT32;

    } else if (strcasecmp(data_type_string, "int32") == 0) {
        return BIONET_RESOURCE_DATA_TYPE_INT32;

    } else if (strcasecmp(data_type_string, "float") == 0) {
        return BIONET_RESOURCE_DATA_TYPE_FLOAT;

    } else if (strcasecmp(data_type_string, "double") == 0) {
        return BIONET_RESOURCE_DATA_TYPE_DOUBLE;

    } else if (strcasecmp(data_type_string, "string") == 0) {
        return BIONET_RESOURCE_DATA_TYPE_STRING;

    }

    return BIONET_RESOURCE_DATA_TYPE_INVALID;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
