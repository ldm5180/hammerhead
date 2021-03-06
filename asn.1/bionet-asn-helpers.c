
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "bionet-asn.h"
#include "bionet-util.h"
#include "util/protected.h"

#ifdef __WIN32
static struct tm * gmtime_r(const time_t *timep, struct tm* _tm) {
    struct tm * ret = gmtime(timep);
    if(ret){
        memcpy(_tm, ret, sizeof(struct tm));
        return _tm;
    } else {
        return NULL;
    }
    
}
#endif

static int _count_bytes(const void *buffer, size_t size, void * voidp) {
    ssize_t *pBytes = (ssize_t*)voidp;

    (*pBytes) += size;

    return size;
}

ssize_t der_encoded_size(
        asn_TYPE_descriptor_t *type_descriptor,
        void *struct_ptr)
{
    ssize_t bytes = 0;
    asn_enc_rval_t asn_r;

    asn_r = type_descriptor->der_encoder(type_descriptor,
            struct_ptr,	/* Pointer to the destination structure */
            0, 0,
            _count_bytes, (void*)&bytes);

    if (asn_r.encoded == -1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "der_encoded_size(): error with der_encode(): %s", 
                asn_r.failed_type ? asn_r.failed_type->name : "unknown");
        return -1;
    }

    return bytes;
}


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
    Datapoint_t *asn_datapoint;

    asn_datapoint = (Datapoint_t *)calloc(1, sizeof(Datapoint_t));
    if (asn_datapoint == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_to_asn(): out of memory");
        return NULL;
    }

    return bionet_datapoint_to_asn_r(d, asn_datapoint);
}

Datapoint_t *bionet_datapoint_to_asn_r(bionet_datapoint_t *d, Datapoint_t * asn_datapoint) {
    int r;
    bionet_value_t *dval = bionet_datapoint_get_value(d);
    bionet_resource_t * resource = bionet_value_get_resource(dval);
    bionet_resource_data_type_t datatype = bionet_resource_get_data_type(resource);
    bionet_value_t *value;

    r = bionet_timeval_to_GeneralizedTime(bionet_datapoint_get_timestamp(d), &asn_datapoint->timestamp);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_to_asn(): error setting timestamp of Datapoint: %s", strerror(errno));
        goto cleanup;
    }

    value = bionet_datapoint_get_value(d);

    switch (datatype) {
        case BIONET_RESOURCE_DATA_TYPE_BINARY: {
            asn_datapoint->value.present = Value_PR_binary_v;
            bionet_value_get_binary(value, (int *)&asn_datapoint->value.choice.binary_v);
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT8: {
            asn_datapoint->value.present = Value_PR_uint8_v;
            bionet_value_get_uint8(value, (uint8_t *)&asn_datapoint->value.choice.uint8_v);
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT8: {
            asn_datapoint->value.present = Value_PR_int8_v;
            bionet_value_get_int8(value, (int8_t *)&asn_datapoint->value.choice.int8_v);
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT16: {
            asn_datapoint->value.present = Value_PR_uint16_v;
            bionet_value_get_uint16(value, (uint16_t *)&asn_datapoint->value.choice.uint16_v);
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT16: {
            asn_datapoint->value.present = Value_PR_int16_v;
            bionet_value_get_int16(value, (int16_t *)&asn_datapoint->value.choice.int16_v);
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT32: {
            int r;
            asn_datapoint->value.present = Value_PR_uint32_v;
	    uint32_t content;
	    bionet_value_get_uint32(value, &content);
            r = asn_long2INTEGER(&asn_datapoint->value.choice.uint32_v, (long)content);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_to_asn(): error making INTEGER for Datapoint value %u", content);
                goto cleanup;
            }
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT32: {
            asn_datapoint->value.present = Value_PR_int32_v;
            bionet_value_get_int32(value, (int32_t *)&asn_datapoint->value.choice.int32_v);
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_FLOAT: {
            asn_datapoint->value.present = Value_PR_real;
	    float tmp;
            bionet_value_get_float(value, &tmp);
	    asn_datapoint->value.choice.real = tmp;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_DOUBLE: {
            asn_datapoint->value.present = Value_PR_real;
	    double tmp;
            bionet_value_get_double(value, &tmp);
	    asn_datapoint->value.choice.real = tmp;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_STRING: {
            int r;
            asn_datapoint->value.present = Value_PR_string;
	    char * content;
            bionet_value_get_str(value, &content);
            r = OCTET_STRING_fromString(&asn_datapoint->value.choice.string, content);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_to_asn(): error making OCTET_STRING for Datapoint String value '%s'", content);
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
    bionet_value_t *value = NULL;
    struct timeval timestamp;
    int r;

    r = bionet_GeneralizedTime_to_timeval(&asn_datapoint->timestamp, &timestamp);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_asn_to_datapoint(): error getting timestamp of ASN Datapoint: %s", strerror(errno));
        return NULL;
    }

    switch (bionet_resource_get_data_type(resource)) {
    case BIONET_RESOURCE_DATA_TYPE_BINARY: {
	if (asn_datapoint->value.choice.binary_v == 0) {
	    value = bionet_value_new_binary(resource, 0);
	} else {
	    value = bionet_value_new_binary(resource, 1);
	}
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT8: {
	value = bionet_value_new_uint8(resource, (uint8_t)asn_datapoint->value.choice.uint8_v);
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT8: {
	value = bionet_value_new_int8(resource, (int8_t)asn_datapoint->value.choice.int8_v);
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT16: {
	value = bionet_value_new_uint16(resource, (uint16_t)asn_datapoint->value.choice.uint16_v);
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT16: {
	value = bionet_value_new_int16(resource, (int16_t)asn_datapoint->value.choice.int16_v);
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_UINT32: {
	int r;
	long l;
	r = asn_INTEGER2long(&asn_datapoint->value.choice.uint32_v, &l);
	if (r != 0) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_asn_to_datapoint(): error converting ASN INTEGER to native Datapoint value");
	    return NULL;
	}
	value = bionet_value_new_uint32(resource, (uint32_t)l);
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_INT32: {
	value = bionet_value_new_int32(resource, (int32_t)asn_datapoint->value.choice.int32_v);
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_FLOAT: {
	value = bionet_value_new_float(resource, (float)asn_datapoint->value.choice.real);
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_DOUBLE: {
	value = bionet_value_new_double(resource, (double)asn_datapoint->value.choice.real);
	break;
    }
    case BIONET_RESOURCE_DATA_TYPE_STRING: {
	value = bionet_value_new_str(resource, (char *)asn_datapoint->value.choice.string.buf);
	if (value == NULL) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_asn_to_datapoint(): error making string Value from ASN.1 OCTET_STRING '%s'",
		  asn_datapoint->value.choice.string.buf);
	    return NULL;
	}
	break;
    }
    default: {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_asn_to_datapoint(): invalid Resource datatype %d", 
	      bionet_resource_get_data_type(resource));
	return NULL;
    }
    }

    return bionet_datapoint_new(resource, value, &timestamp);
}




Resource_t *bionet_resource_to_asn(bionet_resource_t *resource) {
    Resource_t *asn_resource;
    int r;
    int di;  // "datapoint_index"

    asn_resource = (Resource_t *)calloc(1, sizeof(Resource_t));
    if (asn_resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_to_asn(): out of memory!");
        return NULL;
    }

    r = OCTET_STRING_fromString(&asn_resource->id, bionet_resource_get_id(resource));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_to_asn(): error making OCTET_STRING for Resource-ID %s", 
	      bionet_resource_get_id(resource));
        goto cleanup;
    }

    asn_resource->flavor = bionet_flavor_to_asn(bionet_resource_get_flavor(resource));
    if (asn_resource->flavor == -1) {
      g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	    "bionet_resource_to_asn(): invalid resource flavor %d for Resource %s", 
	    bionet_resource_get_flavor(resource), bionet_resource_get_id(resource));
        goto cleanup;
    }

    asn_resource->datatype = bionet_datatype_to_asn(bionet_resource_get_data_type(resource));
    if (asn_resource->datatype == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_to_asn(): invalid Resource Datatype %d for Resource %s", 
	      bionet_resource_get_data_type(resource), bionet_resource_get_id(resource));
        goto cleanup;
    }

    for (di = 0; di < bionet_resource_get_num_datapoints(resource); di ++) {
        Datapoint_t *asn_datapoint;
        bionet_datapoint_t *d = bionet_resource_get_datapoint_by_index(resource, di);

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
        bionet_resource_add_datapoint(resource, d);
    }

    return resource;
}




bionet_stream_direction_t bionet_asn_to_stream_direction(StreamDirection_t asn_direction) {
    switch (asn_direction) {
        case StreamDirection_producer: return BIONET_STREAM_DIRECTION_PRODUCER;
        case StreamDirection_consumer: return BIONET_STREAM_DIRECTION_CONSUMER;
        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_asn_to_stream_direction(): invalid asn StreamDirection %ld", asn_direction);
            return BIONET_STREAM_DIRECTION_INVALID;
        }
    }
}


StreamDirection_t bionet_stream_direction_to_asn(bionet_stream_direction_t direction) {
    switch (direction) {
        case BIONET_STREAM_DIRECTION_PRODUCER:  return StreamDirection_producer;
        case BIONET_STREAM_DIRECTION_CONSUMER:  return StreamDirection_consumer;
        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_direction_to_asn(): invalid stream_direction %d", direction);
            return -1;
        }
    }
}


Stream_t *bionet_stream_to_asn(const bionet_stream_t *stream) {
    Stream_t *asn_stream;
    int r;

    asn_stream = (Stream_t *)calloc(1, sizeof(Stream_t));
    if (asn_stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_to_asn(): out of memory!");
        return NULL;
    }

    r = OCTET_STRING_fromString(&asn_stream->id, bionet_stream_get_id(stream));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_stream_to_asn(): error making OCTET_STRING for Stream-ID %s", 
	      bionet_stream_get_id(stream));
        goto cleanup;
    }

    asn_stream->direction = bionet_stream_direction_to_asn(bionet_stream_get_direction(stream));
    if (asn_stream->direction == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_to_asn(): invalid Stream Direction %d for Stream %s", bionet_stream_get_direction(stream), bionet_stream_get_id(stream));
        goto cleanup;
    }

    r = OCTET_STRING_fromString(&asn_stream->type, bionet_stream_get_type(stream));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_to_asn(): error making OCTET_STRING for Stream Type %s", bionet_stream_get_type(stream));
        goto cleanup;
    }

    return asn_stream;

cleanup:
    ASN_STRUCT_FREE(asn_DEF_Stream, asn_stream);
    return NULL;
}


bionet_stream_t *bionet_asn_to_stream(const Stream_t *asn_stream) {
    bionet_stream_t *stream;
    bionet_stream_direction_t direction;

    direction = bionet_asn_to_stream_direction(asn_stream->direction);
    if (direction == BIONET_STREAM_DIRECTION_INVALID) {
        // an error has been logged
        return NULL;
    }

    stream = bionet_stream_new(
        NULL,  // node
        (const char *)asn_stream->id.buf,
        direction,
        (const char *)asn_stream->type.buf
    );

    return stream;
}




int bionet_node_to_asn(const bionet_node_t *node, Node_t *asn_node) {
    int r;
    int i;


    memset(asn_node, 0x00, sizeof(Node_t));


    // 
    // node id
    //
    r = OCTET_STRING_fromString(&asn_node->id, bionet_node_get_id(node));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_node_to_asn(): error making OCTET_STRING for Node-ID %s", 
	      bionet_node_get_id(node));
        goto cleanup;
    }

    // 
    // the Node's Resources
    //

    for (i = 0; i < bionet_node_get_num_resources(node); i ++) {
        Resource_t *asn_resource;
        bionet_resource_t *resource = bionet_node_get_resource_by_index(node, i);

        asn_resource = bionet_resource_to_asn(resource);
        if (asn_resource == NULL) {
            // an error has been logged already
            goto cleanup;
        }

        r = asn_sequence_add(&asn_node->resources.list, asn_resource);
        if (r != 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_to_asn(): error adding ASN.1 Resource to ASN.1 Node structure: %s", strerror(errno));
            ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_Resource, asn_resource);
            goto cleanup;
        }
    }


    // 
    // the Node's Streams
    //

    for (i = 0; i < bionet_node_get_num_streams(node); i ++) {
        Stream_t *asn_stream;
        bionet_stream_t *stream = bionet_node_get_stream_by_index(node, i);

        asn_stream = bionet_stream_to_asn(stream);
        if (asn_stream == NULL) {
            // an error has been logged already
            goto cleanup;
        }

        r = asn_sequence_add(&asn_node->streams.list, asn_stream);
        if (r != 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_to_asn(): error adding ASN.1 Stream to ASN.1 Node structure: %s", strerror(errno));
            ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_Stream, asn_stream);
            goto cleanup;
        }
    }


    return 0;


cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_Node, asn_node);
    return -1;
}




int bionet_node_to_asnbuf(const bionet_node_t *node, bionet_asn_buffer_t *buf) {
    H2C_Message_t m;
    Node_t *asn_node;
    asn_enc_rval_t asn_r;
    int r;


    buf->buf = NULL;
    buf->size = 0;

    memset(&m, 0x00, sizeof(H2C_Message_t));
    m.present = H2C_Message_PR_newNode;
    asn_node = &m.choice.newNode;


    r = bionet_node_to_asn(node, asn_node);
    if (r != 0) {
        // an error has been logged
        return -1;
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
        goto cleanup;
    }

    return 0;


cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_H2C_Message, &m);
    return -1;
}


bionet_hab_t *bionet_asn_to_hab(const HardwareAbstractor_t *asn_hab) {
    int i;
    bionet_hab_t *hab;



    hab = bionet_hab_new((char *)asn_hab->type.buf, (char*)asn_hab->id.buf);
    if (hab == NULL) {
        // an error has been logged
        return NULL;
    }


    // 
    // the Hab's Nodes
    //

    for (i = 0; i < asn_hab->nodes.list.count; i ++) {
        bionet_node_t *node;
        Node_t *asn_node = asn_hab->nodes.list.array[i];
        int r;

        node = bionet_asn_to_node(asn_node, hab);
        if (node == NULL) {
            // an error has been logged already
            goto cleanup;
        }

        r = bionet_hab_add_node(hab, node);
        if (r != 0) {
            // an error's been logged
            bionet_node_free(node);
            goto cleanup;
        }
    }

    return hab;


cleanup:
    bionet_hab_free(hab);
    return NULL;
}

bionet_node_t *bionet_asn_to_node(const Node_t *asn_node, bionet_hab_t *hab) {
    int i;
    bionet_node_t *node;


    // 
    // make the Node and grab the Node-ID
    //

    node = bionet_node_new(hab, (char *)asn_node->id.buf);
    if (node == NULL) {
        // an error has been logged
        return NULL;
    }


    // 
    // the Node's Resources
    //

    for (i = 0; i < asn_node->resources.list.count; i ++) {
        bionet_resource_t *resource;
        Resource_t *asn_resource = asn_node->resources.list.array[i];
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


    // 
    // the Node's Streams
    //

    for (i = 0; i < asn_node->streams.list.count; i ++) {
        bionet_stream_t *stream;
        Stream_t *asn_stream = asn_node->streams.list.array[i];
        int r;

        stream = bionet_asn_to_stream(asn_stream);
        if (stream == NULL) {
            // an error has been logged already
            goto cleanup;
        }

        r = bionet_node_add_stream(node, stream);
        if (r != 0) {
            // an error's been logged
            bionet_stream_free(stream);
            goto cleanup;
        }
    }

    return node;


cleanup:
    bionet_node_free(node);
    return NULL;
}


int bionet_resource_metadata_to_asnbuf(bionet_resource_t *resource, bionet_asn_buffer_t *buf) {
    H2C_Message_t m;
    ResourceMetadata_t *rm;
    asn_enc_rval_t asn_r;
    int r;


    buf->buf = NULL;
    buf->size = 0;

    memset(&m, 0x00, sizeof(H2C_Message_t));
    m.present = H2C_Message_PR_resourceMetadata;
    rm = &m.choice.resourceMetadata;


    // node id
    r = OCTET_STRING_fromString(&rm->nodeId, bionet_node_get_id(bionet_resource_get_node(resource)));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_metadata_to_asn(): error making OCTET_STRING for Node-ID %s", 
	      bionet_node_get_id(bionet_resource_get_node(resource)));
        goto cleanup;
    }

    // resource id
    r = OCTET_STRING_fromString(&rm->resourceId, bionet_resource_get_id(resource));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_metadata_to_asn(): error making OCTET_STRING for Resource-ID %s", 
	      bionet_resource_get_id(resource));
        goto cleanup;
    }

    // resource flavor
    rm->flavor = bionet_flavor_to_asn(bionet_resource_get_flavor(resource));
    if (rm->flavor == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_metadata_to_asn(): error making ResourceFlavor from Resource Flavor %d", 
	      bionet_resource_get_flavor(resource));
        goto cleanup;
    }

    // resource data type
    rm->datatype = bionet_datatype_to_asn(bionet_resource_get_data_type(resource));
    if (rm->datatype == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_metadata_to_asn(): error making ResourceDatatype from Resource Datatype %d", 
	      bionet_resource_get_data_type(resource));
        goto cleanup;
    }

    // 
    // serialize the H2C-Message
    //

    asn_r = der_encode(&asn_DEF_H2C_Message, &m, bionet_accumulate_asn_buffer, buf);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_H2C_Message, &m);
    if (asn_r.encoded == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_metadata_to_asnbuf(): error with der_encode(): %s", strerror(errno));
        if (buf->buf != NULL) {
            free(buf->buf);
            buf->buf = NULL;
        }
        goto cleanup;
    }

    return 0;


cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_H2C_Message, &m);
    return -1;
}


int bionet_resource_datapoints_to_asnbuf(bionet_resource_t *resource, 
					 bionet_asn_buffer_t *buf, 
					 int dirty_only, 
					 GHashTable * recent_dps) {
    H2C_Message_t m;
    ResourceDatapoints_t *rd;
    asn_enc_rval_t asn_r;
    int r;
    int di;


    buf->buf = NULL;
    buf->size = 0;

    memset(&m, 0x00, sizeof(H2C_Message_t));
    m.present = H2C_Message_PR_datapointsUpdate;
    rd = &m.choice.datapointsUpdate;

    // node id
    r = OCTET_STRING_fromString(&rd->nodeId, bionet_node_get_id(bionet_resource_get_node(resource)));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_datapoints_to_asn(): error making OCTET_STRING for Node-ID %s", 
	      bionet_node_get_id(bionet_resource_get_node(resource)));
        goto cleanup;
    }

    // resource id
    r = OCTET_STRING_fromString(&rd->resourceId, bionet_resource_get_id(resource));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_datapoints_to_asn(): error making OCTET_STRING for Resource-ID %s", 
	      bionet_resource_get_id(resource));
        goto cleanup;
    }

    // 
    // all the dirty datapoints
    //

    for (di = 0; di < bionet_resource_get_num_datapoints(resource); di++) {
        bionet_datapoint_t *d = bionet_resource_get_datapoint_by_index(resource, di);
        Datapoint_t *asn_d;

        if (dirty_only && (!bionet_datapoint_is_dirty(d))) continue;

	if (recent_dps) {
	    bionet_datapoint_t * temp_dp = g_hash_table_lookup(recent_dps, resource);
	    if (temp_dp) {
		d = temp_dp;
	    } 
	}

        asn_d = bionet_datapoint_to_asn(d);

        r = asn_sequence_add(&rd->newDatapoints.list, asn_d);
        if (r != 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_datapoints_to_asn(): error adding Datapoint to Resource: %s", strerror(errno));
            goto cleanup;
        }
    }


    // 
    // serialize the H2C-Message
    //

    asn_r = der_encode(&asn_DEF_H2C_Message, &m, bionet_accumulate_asn_buffer, buf);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_H2C_Message, &m);
    if (asn_r.encoded == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_datapoints_to_asnbuf(): error with der_encode(): %s", strerror(errno));
        if (buf->buf != NULL) {
            free(buf->buf);
            buf->buf = NULL;
        }
        goto cleanup;
    }

    return 0;


cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_H2C_Message, &m);
    return -1;
}

int bionet_hab_to_asn(const bionet_hab_t * hab, HardwareAbstractor_t * asn_hab) {
    int r, ni;

    memset(asn_hab, 0x00, sizeof(HardwareAbstractor_t));

    r = OCTET_STRING_fromString(&asn_hab->type, bionet_hab_get_type(hab));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error making OCTET_STRING for HAB-Type %s", __FUNCTION__, bionet_hab_get_type(hab));
        goto cleanup;
    }

    r = OCTET_STRING_fromString(&asn_hab->id, bionet_hab_get_id(hab));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error making OCTET_STRING for HAB-ID %s", __FUNCTION__, bionet_hab_get_id(hab));
        goto cleanup;
    }

    for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni ++) {
        int ri;
        Node_t *asn_node;
        bionet_node_t *node = bionet_hab_get_node_by_index(hab, ni);

        asn_node = (Node_t *)calloc(1, sizeof(Node_t));
        if (asn_node == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): out of memory!", __FUNCTION__);
            return -1;
        }

        r = asn_sequence_add(&asn_hab->nodes.list, asn_node);
        if (r != 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error adding Node to ResourceDatapointReply: %s", __FUNCTION__, strerror(errno));
            return -1;
        }

        r = OCTET_STRING_fromString(&asn_node->id, bionet_node_get_id(node));
        if (r != 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error making OCTET_STRING for Node-ID %s", __FUNCTION__, bionet_node_get_id(node));
            goto cleanup;
        }

        for (ri = 0; ri < bionet_node_get_num_resources(node); ri ++) {
            int di;
            Resource_t *asn_resource;
            bionet_resource_t *resource = bionet_node_get_resource_by_index(node, ri);

            asn_resource = (Resource_t *)calloc(1, sizeof(Resource_t));
            if (asn_resource == NULL) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): out of memory!", __FUNCTION__);
                goto cleanup;
            }

            r = asn_sequence_add(&asn_node->resources.list, asn_resource);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error adding Resource to ResourceDatapointReply: %s", __FUNCTION__, strerror(errno));
                goto cleanup;
            }

            r = OCTET_STRING_fromString(&asn_resource->id, bionet_resource_get_id(resource));
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error making OCTET_STRING for Resource-ID %s", __FUNCTION__, bionet_resource_get_id(resource));
                goto cleanup;
            }

            asn_resource->flavor = bionet_flavor_to_asn(bionet_resource_get_flavor(resource));
            if (asn_resource->flavor == -1) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): resource has invalid flavor", __FUNCTION__);
                goto cleanup;
            }

            asn_resource->datatype = bionet_datatype_to_asn(bionet_resource_get_data_type(resource));
            if (asn_resource->datatype == -1) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): resource has invalid datatype", __FUNCTION__);
                goto cleanup;
            }

            for (di = 0; di < bionet_resource_get_num_datapoints(resource); di ++) {
                bionet_datapoint_t *d = bionet_resource_get_datapoint_by_index(resource, di);
                Datapoint_t *asn_datapoint;

                asn_datapoint = bionet_datapoint_to_asn(d);
                if (asn_datapoint == NULL) {
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): out of memory!", __FUNCTION__);
                    goto cleanup;
                }

                r = asn_sequence_add(&asn_resource->datapoints.list, asn_datapoint);
                if (r != 0) {
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error adding Datapoint to Resource: %s", __FUNCTION__, strerror(errno));
                    goto cleanup;
                }

            }
        }
    }

    return 0;

cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_HardwareAbstractor, asn_hab);
    return -1;
}

