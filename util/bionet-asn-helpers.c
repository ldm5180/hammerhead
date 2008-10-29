
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "bionet-asn.h"
#include "bionet-util.h"


ResourceFlavor_t bionet_flavor_to_asn(bionet_resource_flavor_t flavor) {
    switch (flavor) {
        case BIONET_RESOURCE_FLAVOR_SENSOR:    return ResourceFlavor_sensor;
        case BIONET_RESOURCE_FLAVOR_ACTUATOR:  return ResourceFlavor_actuator;
        case BIONET_RESOURCE_FLAVOR_PARAMETER: return ResourceFlavor_parameter;
        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_flavor_to_asn(): invalid flavor %d", flavor);
            return -1;
        }
    }
}


bionet_resource_flavor_t bionet_asn_to_flavor(ResourceFlavor_t asn_flavor) {
    switch (asn_flavor) {
        case ResourceFlavor_sensor: return BIONET_RESOURCE_FLAVOR_SENSOR;
        case ResourceFlavor_actuator:  return BIONET_RESOURCE_FLAVOR_ACTUATOR;
        case ResourceFlavor_parameter: return BIONET_RESOURCE_FLAVOR_PARAMETER;
        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_asn_to_flavor(): invalid asn flavor %ld", asn_flavor);
            return -1;
        }
    }
}


ResourceDataType_t bionet_datatype_to_asn(bionet_resource_data_type_t datatype) {
    switch (datatype) {
        case BIONET_RESOURCE_DATA_TYPE_BINARY:  return ResourceDataType_binary;
        case BIONET_RESOURCE_DATA_TYPE_UINT8:   return ResourceDataType_uint8;
        case BIONET_RESOURCE_DATA_TYPE_INT8:    return ResourceDataType_int8;
        case BIONET_RESOURCE_DATA_TYPE_UINT16:  return ResourceDataType_uint16;
        case BIONET_RESOURCE_DATA_TYPE_INT16:   return ResourceDataType_int16;
        case BIONET_RESOURCE_DATA_TYPE_UINT32:  return ResourceDataType_uint32;
        case BIONET_RESOURCE_DATA_TYPE_INT32:   return ResourceDataType_int32;
        case BIONET_RESOURCE_DATA_TYPE_FLOAT:   return ResourceDataType_float;
        case BIONET_RESOURCE_DATA_TYPE_DOUBLE:  return ResourceDataType_double;
        case BIONET_RESOURCE_DATA_TYPE_STRING:  return ResourceDataType_string;
        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datatype_to_asn(): invalid datatype %d", datatype);
            return -1;
        }
    }
}


bionet_resource_data_type_t bionet_asn_to_datatype(ResourceDataType_t asn_datatype) {
    switch (asn_datatype) {
        case ResourceDataType_binary:  return BIONET_RESOURCE_DATA_TYPE_BINARY;
        case ResourceDataType_uint8:   return BIONET_RESOURCE_DATA_TYPE_UINT8;
        case ResourceDataType_int8:    return BIONET_RESOURCE_DATA_TYPE_INT8;
        case ResourceDataType_uint16:  return BIONET_RESOURCE_DATA_TYPE_UINT16;
        case ResourceDataType_int16:   return BIONET_RESOURCE_DATA_TYPE_INT16;
        case ResourceDataType_uint32:  return BIONET_RESOURCE_DATA_TYPE_UINT32;
        case ResourceDataType_int32:   return BIONET_RESOURCE_DATA_TYPE_INT32;
        case ResourceDataType_float:   return BIONET_RESOURCE_DATA_TYPE_FLOAT;
        case ResourceDataType_double:  return BIONET_RESOURCE_DATA_TYPE_DOUBLE;
        case ResourceDataType_string:  return BIONET_RESOURCE_DATA_TYPE_STRING;
        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_asn_to_datatype(): invalid datatype %ld", asn_datatype);
            return -1;
        }
    }
}


int bionet_datapoint_to_asn(bionet_datapoint_t *d, bionet_resource_data_type_t datatype, Datapoint_t *asn_datapoint) {
    int r;

    r = bionet_timeval_to_GeneralizedTime(&d->timestamp, &asn_datapoint->timestamp);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_to_asn(): error setting timestamp of Datapoint: %s", strerror(errno));
        return -1;
    }

    switch (datatype) {
        case BIONET_RESOURCE_DATA_TYPE_BINARY: {
            asn_datapoint->value.present = Value_PR_binary_v;
            asn_datapoint->value.choice.binary_v = d->value.binary_v;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT8: {
            asn_datapoint->value.present = Value_PR_uint8_v;
            asn_datapoint->value.choice.uint8_v = d->value.uint8_v;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT8: {
            asn_datapoint->value.present = Value_PR_int8_v;
            asn_datapoint->value.choice.int8_v = d->value.int8_v;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT16: {
            asn_datapoint->value.present = Value_PR_uint16_v;
            asn_datapoint->value.choice.uint16_v = d->value.uint16_v;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT16: {
            asn_datapoint->value.present = Value_PR_int16_v;
            asn_datapoint->value.choice.int16_v = d->value.int16_v;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT32: {
            int r;
            asn_datapoint->value.present = Value_PR_uint32_v;
            r = asn_long2INTEGER(&asn_datapoint->value.choice.uint32_v, (long)d->value.uint32_v);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_to_asn(): error making INTEGER for Datapoint value %u", d->value.uint32_v);
                return -1;
            }
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT32: {
            asn_datapoint->value.present = Value_PR_int32_v;
            asn_datapoint->value.choice.int32_v = d->value.int32_v;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_FLOAT: {
            asn_datapoint->value.present = Value_PR_real;
            asn_datapoint->value.choice.real = d->value.float_v;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_DOUBLE: {
            asn_datapoint->value.present = Value_PR_real;
            asn_datapoint->value.choice.real = d->value.double_v;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_STRING: {
            int r;
            asn_datapoint->value.present = Value_PR_string;
            r = OCTET_STRING_fromString(&asn_datapoint->value.choice.string, d->value.string_v);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_to_asn(): error making OCTET_STRING for Datapoint String value '%s'", d->value.string_v);
                return -1;
            }
            return 0;
        }
        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_to_asn(): invalid datatype %d", datatype);
            return -1;
        }
    }
}


int bionet_asn_to_datapoint(Datapoint_t *asn_datapoint, bionet_datapoint_t *d, bionet_resource_data_type_t datatype) {
    int r;

    r = bionet_GeneralizedTime_to_timeval(&asn_datapoint->timestamp, &d->timestamp);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_asn_to_datapoint(): error setting timestamp of Datapoint: %s", strerror(errno));
        return -1;
    }

    switch (datatype) {
        case BIONET_RESOURCE_DATA_TYPE_BINARY: {
            d->value.binary_v = asn_datapoint->value.choice.binary_v;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT8: {
            d->value.uint8_v = asn_datapoint->value.choice.uint8_v;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT8: {
            d->value.int8_v = asn_datapoint->value.choice.int8_v;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT16: {
            d->value.uint16_v = asn_datapoint->value.choice.uint16_v;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT16: {
            d->value.int16_v = asn_datapoint->value.choice.int16_v;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT32: {
            int r;
            long l;
            r = asn_INTEGER2long(&asn_datapoint->value.choice.uint32_v, &l);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_asn_to_datapoint(): error converting ASN INTEGER to native Datapoint value");
                return -1;
            }
            d->value.uint32_v = l;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT32: {
            d->value.int32_v = asn_datapoint->value.choice.int32_v;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_FLOAT: {
            d->value.float_v = asn_datapoint->value.choice.real;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_DOUBLE: {
            d->value.double_v = asn_datapoint->value.choice.real;
            return 0;
        }
        case BIONET_RESOURCE_DATA_TYPE_STRING: {
            if (d->value.string_v != NULL) free(d->value.string_v);
            d->value.string_v = strdup((char *)asn_datapoint->value.choice.string.buf);
            if (d->value.string_v == NULL) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_asn_to_datapoint(): error making native string from ASN.1 OCTET_STRING '%s'", asn_datapoint->value.choice.string.buf);
                return -1;
            }
            return 0;
        }
        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_asn_to_datapoint(): invalid datatype %d", datatype);
            return -1;
        }
    }
}

