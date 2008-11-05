
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "bionet-asn.h"
#include "bionet-util.h"




int bionet_accumulate_asn_buffer(const void *new_buffer, size_t new_size, void *buffer_as_voidp) {
    bionet_asn_buffer_t *buffer = buffer_as_voidp;

    buffer->buf = realloc(buffer->buf, (buffer->size + new_size));
    if (buffer->buf == NULL) {
        return -1;
    }

    memcpy((buffer->buf + buffer->size), new_buffer, new_size);
    buffer->size += new_size;

    return new_size;
}




int bionet_GeneralizedTime_to_timeval(const GeneralizedTime_t *gt, struct timeval *tv) {
    time_t r;
    int usec;

    r = asn_GT2time_prec(gt, &usec, 6, NULL, 0);
    if (r == -1) {
        return -1;
    }

    tv->tv_sec = r;
    tv->tv_usec = usec;

    return 0;
}


int bionet_timeval_to_GeneralizedTime(const struct timeval *tv, GeneralizedTime_t *gt) {
    struct tm tm;
    GeneralizedTime_t *result;

    if (gmtime_r(&tv->tv_sec, &tm) == NULL) {
        return -1;
    }

    result = asn_time2GT_frac(gt, &tm, tv->tv_usec, 6, 1);
    if (result == NULL) {
        return -1;
    }

    return 0;
}




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
            return BIONET_RESOURCE_FLAVOR_INVALID;
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
            return BIONET_RESOURCE_DATA_TYPE_INVALID;
        }
    }
}


Datapoint_t *bionet_datapoint_to_asn(bionet_datapoint_t *d) {
    int r;
    bionet_resource_data_type_t datatype = d->resource->data_type;
    Datapoint_t *asn_datapoint;

    asn_datapoint = (Datapoint_t *)calloc(1, sizeof(Datapoint_t));
    if (asn_datapoint == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_to_asn(): out of memory");
        return NULL;
    }

    r = bionet_timeval_to_GeneralizedTime(&d->timestamp, &asn_datapoint->timestamp);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_to_asn(): error setting timestamp of Datapoint: %s", strerror(errno));
        goto cleanup;
    }

    switch (datatype) {
        case BIONET_RESOURCE_DATA_TYPE_BINARY: {
            asn_datapoint->value.present = Value_PR_binary_v;
            asn_datapoint->value.choice.binary_v = d->value.binary_v;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT8: {
            asn_datapoint->value.present = Value_PR_uint8_v;
            asn_datapoint->value.choice.uint8_v = d->value.uint8_v;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT8: {
            asn_datapoint->value.present = Value_PR_int8_v;
            asn_datapoint->value.choice.int8_v = d->value.int8_v;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT16: {
            asn_datapoint->value.present = Value_PR_uint16_v;
            asn_datapoint->value.choice.uint16_v = d->value.uint16_v;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT16: {
            asn_datapoint->value.present = Value_PR_int16_v;
            asn_datapoint->value.choice.int16_v = d->value.int16_v;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT32: {
            int r;
            asn_datapoint->value.present = Value_PR_uint32_v;
            r = asn_long2INTEGER(&asn_datapoint->value.choice.uint32_v, (long)d->value.uint32_v);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_to_asn(): error making INTEGER for Datapoint value %u", d->value.uint32_v);
                goto cleanup;
            }
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT32: {
            asn_datapoint->value.present = Value_PR_int32_v;
            asn_datapoint->value.choice.int32_v = d->value.int32_v;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_FLOAT: {
            asn_datapoint->value.present = Value_PR_real;
            asn_datapoint->value.choice.real = d->value.float_v;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_DOUBLE: {
            asn_datapoint->value.present = Value_PR_real;
            asn_datapoint->value.choice.real = d->value.double_v;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_STRING: {
            int r;
            asn_datapoint->value.present = Value_PR_string;
            r = OCTET_STRING_fromString(&asn_datapoint->value.choice.string, d->value.string_v);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_to_asn(): error making OCTET_STRING for Datapoint String value '%s'", d->value.string_v);
                goto cleanup;
            }
            break;
        }
        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_to_asn(): invalid datatype %d", datatype);
            goto cleanup;
        }
    }

    return asn_datapoint;


cleanup:
    ASN_STRUCT_FREE(asn_DEF_Datapoint, asn_datapoint);
    return NULL;
}




bionet_datapoint_t *bionet_asn_to_datapoint(Datapoint_t *asn_datapoint, bionet_resource_t *resource) {
    bionet_datapoint_value_t value;
    struct timeval timestamp;
    int r;

    r = bionet_GeneralizedTime_to_timeval(&asn_datapoint->timestamp, &timestamp);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_asn_to_datapoint(): error getting timestamp of ASN Datapoint: %s", strerror(errno));
        return NULL;
    }

    switch (resource->data_type) {
        case BIONET_RESOURCE_DATA_TYPE_BINARY: {
            value.binary_v = asn_datapoint->value.choice.binary_v;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT8: {
            value.uint8_v = asn_datapoint->value.choice.uint8_v;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT8: {
            value.int8_v = asn_datapoint->value.choice.int8_v;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT16: {
            value.uint16_v = asn_datapoint->value.choice.uint16_v;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT16: {
            value.int16_v = asn_datapoint->value.choice.int16_v;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT32: {
            int r;
            long l;
            r = asn_INTEGER2long(&asn_datapoint->value.choice.uint32_v, &l);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_asn_to_datapoint(): error converting ASN INTEGER to native Datapoint value");
                return NULL;
            }
            value.uint32_v = l;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT32: {
            value.int32_v = asn_datapoint->value.choice.int32_v;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_FLOAT: {
            value.float_v = asn_datapoint->value.choice.real;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_DOUBLE: {
            value.double_v = asn_datapoint->value.choice.real;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_STRING: {
            value.string_v = strdup((char *)asn_datapoint->value.choice.string.buf);
            if (value.string_v == NULL) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_asn_to_datapoint(): error making native string from ASN.1 OCTET_STRING '%s'", asn_datapoint->value.choice.string.buf);
                return NULL;
            }
            break;
        }
        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_asn_to_datapoint(): invalid Resource datatype %d", resource->data_type);
            return NULL;
        }
    }

    return bionet_datapoint_new(resource, &value, &timestamp);
}




Resource_t *bionet_resource_to_asn(const bionet_resource_t *resource) {
    Resource_t *asn_resource;
    int r;
    int di;  // "datapoint_index"

    asn_resource = (Resource_t *)calloc(1, sizeof(Resource_t));
    if (asn_resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_to_asn(): out of memory!");
        return NULL;
    }

    r = OCTET_STRING_fromString(&asn_resource->id, resource->id);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_to_asn(): error making OCTET_STRING for Resource-ID %s", resource->id);
        goto cleanup;
    }

    asn_resource->flavor = bionet_flavor_to_asn(resource->flavor);
    if (asn_resource->flavor == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_to_asn(): invalid resource flavor %d for Resource %s", resource->flavor, resource->id);
        goto cleanup;
    }

    asn_resource->datatype = bionet_datatype_to_asn(resource->data_type);
    if (asn_resource->datatype == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_to_asn(): invalid Resource Datatype %d for Resource %s", resource->data_type, resource->id);
        goto cleanup;
    }


    for (di = 0; di < bionet_resource_get_num_datapoints(resource); di ++) {
        bionet_datapoint_t *d = bionet_resource_get_datapoint_by_index(resource, di);
        Datapoint_t *asn_datapoint;

        asn_datapoint = bionet_datapoint_to_asn(d);
        if (asn_datapoint == NULL) {
            // an error has been logged
            goto cleanup;
        }

        r = asn_sequence_add(&asn_resource->datapoints.list, asn_datapoint);
        if (r != 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_to_asn(): error adding Datapoint to Resource: %s", strerror(errno));
            goto cleanup;
        }
    }

    return asn_resource;


cleanup:
    ASN_STRUCT_FREE(asn_DEF_Resource, asn_resource);
    return NULL;
}




bionet_resource_t *bionet_asn_to_resource(const Resource_t *asn_resource) {
    bionet_resource_flavor_t flavor;
    bionet_resource_data_type_t data_type;
    bionet_resource_t *resource;

    int di;  // "datapoint_index"


    flavor = bionet_asn_to_flavor(asn_resource->flavor);
    if (flavor == BIONET_RESOURCE_FLAVOR_INVALID) {
        // an error has been logged
        return NULL;
    }

    data_type = bionet_asn_to_datatype(asn_resource->datatype);
    if (data_type == -1) {
        // an error has been logged
        return NULL;
    }

    resource = bionet_resource_new(NULL, data_type, flavor, (char *)asn_resource->id.buf);
    if (resource == NULL) {
        // an error has been logged
        return NULL;
    }


    for (di = 0; di < asn_resource->datapoints.list.count; di ++) {
        Datapoint_t *asn_datapoint = asn_resource->datapoints.list.array[di];
        bionet_datapoint_t *d = bionet_asn_to_datapoint(asn_datapoint, resource);
        bionet_resource_add_existing_datapoint(resource, d);
    }

    return resource;
}




int bionet_node_to_asnbuf(const bionet_node_t *node, bionet_asn_buffer_t *buf) {
    H2C_Message_t m;
    Node_t *asn_node;
    asn_enc_rval_t asn_r;
    int r;
    int ri;  // "resource index"


    buf->buf = NULL;
    buf->size = 0;

    memset(&m, 0x00, sizeof(H2C_Message_t));
    m.present = H2C_Message_PR_newNode;
    asn_node = &m.choice.newNode;


    // 
    // node id
    //

    r = OCTET_STRING_fromString(&asn_node->id, node->id);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_to_asnbuf(): error making OCTET_STRING for Node-ID %s", node->id);
        ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_H2C_Message, &m);
        return -1;
    }


    // 
    // the Node's Resources
    //

    for (ri = 0; ri < bionet_node_get_num_resources(node); ri ++) {
        Resource_t *asn_resource;
        bionet_resource_t *resource = bionet_node_get_resource_by_index(node, ri);

        asn_resource = bionet_resource_to_asn(resource);
        if (asn_resource == NULL) {
            // an error has been logged already
            goto cleanup;
        }

        r = asn_sequence_add(&asn_node->resources.list, asn_resource);
        if (r != 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_to_asnbuf(): error adding Resource to newNode H2C-Message: %s", strerror(errno));
            ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_Resource, asn_resource);
            goto cleanup;
        }
    }


    // 
    // serialize the newNode Message
    //

    asn_r = der_encode(&asn_DEF_H2C_Message, &m, bionet_accumulate_asn_buffer, buf);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_H2C_Message, &m);
    if (asn_r.encoded == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_to_asnbuf(): error with der_encode(): %s", strerror(errno));
        if (buf->buf != NULL) {
            free(buf->buf);
            buf->buf = NULL;
        }
        return -1;
    }

    return 0;


cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_H2C_Message, &m);
    return -1;
}




bionet_node_t *bionet_asn_to_node(const Node_t *asn_node) {
    int ri;
    bionet_node_t *node;


    // 
    // make the Node and grab the Node-ID
    //

    node = bionet_node_new(NULL, (char *)asn_node->id.buf);
    if (node == NULL) {
        // an error has been logged
        return NULL;
    }


    // 
    // the Node's Resources
    //

    for (ri = 0; ri < asn_node->resources.list.count; ri ++) {
        bionet_resource_t *resource;
        Resource_t *asn_resource = asn_node->resources.list.array[ri];
        int r;

        resource = bionet_asn_to_resource(asn_resource);
        if (resource == NULL) {
            // an error has been logged already
            goto cleanup;
        }

        r = bionet_node_add_resource(node, resource);
        if (r != 0) {
            // an error's been logged
            bionet_resource_free(resource);
            goto cleanup;
        }
    }

    return node;


cleanup:
    bionet_node_free(node);
    return NULL;
}

