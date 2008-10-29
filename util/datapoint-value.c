
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#define _GNU_SOURCE  // for strtof(3)

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet-util.h"




const char *bionet_datapoint_value_to_string(const bionet_datapoint_t *datapoint) {
    return bionet_datapoint_value_to_string_isolated(datapoint->resource->data_type, &datapoint->value);
}


const char *bionet_datapoint_value_to_string_isolated(bionet_resource_data_type_t data_type, const bionet_datapoint_value_t *value) {
    int r;
    static char val_str[512];


    switch (data_type) {

        case BIONET_RESOURCE_DATA_TYPE_BINARY: {
            r = snprintf(val_str, sizeof(val_str), "%d", (int)value->binary_v);
            if (r >= sizeof(val_str)) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_value_to_string_isolated(): value of datapoint is too big to fit in output string!");
                return NULL;
            }
            return val_str;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT8: {
            r = snprintf(val_str, sizeof(val_str), "%hhu", value->uint8_v);
            if (r >= sizeof(val_str)) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_value_to_string_isolated(): value of datapoint is too big to fit in output string!");
                return NULL;
            }
            return val_str;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT8: {
            r = snprintf(val_str, sizeof(val_str), "%hhd", value->int8_v);
            if (r >= sizeof(val_str)) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_value_to_string_isolated(): value of datapoint is too big to fit in output string!");
                return NULL;
            }
            return val_str;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT16: {
            r = snprintf(val_str, sizeof(val_str), "%hu", value->uint16_v);
            if (r >= sizeof(val_str)) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_value_to_string_isolated(): value of datapoint is too big to fit in output string!");
                return NULL;
            }
            return val_str;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT16: {
            r = snprintf(val_str, sizeof(val_str), "%hd", value->int16_v);
            if (r >= sizeof(val_str)) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_value_to_string_isolated(): value of datapoint is too big to fit in output string!");
                return NULL;
            }
            return val_str;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT32: {
            r = snprintf(val_str, sizeof(val_str), "%u", value->uint32_v);
            if (r >= sizeof(val_str)) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_value_to_string_isolated(): value of datapoint is too big to fit in output string!");
                return NULL;
            }
            return val_str;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT32: {
            r = snprintf(val_str, sizeof(val_str), "%d", value->int32_v);
            if (r >= sizeof(val_str)) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_value_to_string_isolated(): value of datapoint is too big to fit in output string!");
                return NULL;
            }
            return val_str;
        }

        case BIONET_RESOURCE_DATA_TYPE_FLOAT: {
            r = snprintf(val_str, sizeof(val_str), "%.7f", value->float_v);
            if (r >= sizeof(val_str)) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_value_to_string_isolated(): value of datapoint is too big to fit in output string!");
                return NULL;
            }
            return val_str;
        }

        case BIONET_RESOURCE_DATA_TYPE_DOUBLE: {
            r = snprintf(val_str, sizeof(val_str), "%.15f", value->double_v);
            if (r >= sizeof(val_str)) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_value_to_string_isolated(): value of datapoint is too big to fit in output string!");
                return NULL;
            }
            return val_str;
        }

        case BIONET_RESOURCE_DATA_TYPE_STRING: {
            return value->string_v;
        }

        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_value_to_string_isolated():  unknown data-type %d", data_type);
            return NULL;
        }
    }


    // not reached
    return NULL;
}




#if 0
int bionet_resource_value_from_pointer(const void *value, bionet_resource_t *dest_resource) {
    if (value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_value_from_pointer():  NULL value passed in");
        errno = EINVAL;
        return -1;
    }
    if (dest_resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_value_from_pointer():  NULL dest_resource passed in");
        errno = EINVAL;
        return -1;
    }

    switch (dest_resource->data_type) {
        case BIONET_RESOURCE_DATA_TYPE_BINARY:
            dest_resource->value.binary_v = *(int *)value;
            dest_resource->dirty = 1;
            return 0;

        case BIONET_RESOURCE_DATA_TYPE_UINT8:
            dest_resource->value.uint8_v = *(uint8_t *)value;
            dest_resource->dirty = 1;
            return 0;

        case BIONET_RESOURCE_DATA_TYPE_INT8:
            dest_resource->value.int8_v = *(int8_t *)value;
            dest_resource->dirty = 1;
            return 0;

        case BIONET_RESOURCE_DATA_TYPE_UINT16:
            dest_resource->value.uint16_v = *(uint16_t *)value;
            dest_resource->dirty = 1;
            return 0;

        case BIONET_RESOURCE_DATA_TYPE_INT16:
            dest_resource->value.int16_v = *(int16_t *)value;
            dest_resource->dirty = 1;
            return 0;

        case BIONET_RESOURCE_DATA_TYPE_UINT32:
            dest_resource->value.uint32_v = *(uint32_t *)value;
            dest_resource->dirty = 1;
            return 0;

        case BIONET_RESOURCE_DATA_TYPE_INT32:
            dest_resource->value.int32_v = *(int32_t *)value;
            dest_resource->dirty = 1;
            return 0;

        case BIONET_RESOURCE_DATA_TYPE_FLOAT:
            dest_resource->value.float_v = *(float *)value;
            dest_resource->dirty = 1;
            return 0;

        case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
            dest_resource->value.double_v = *(double *)value;
            dest_resource->dirty = 1;
            return 0;

        case BIONET_RESOURCE_DATA_TYPE_STRING: {
            int i;
            char *string = (char*)value;

            for (i = 0; string[i] != '\0'; i++) {
                if (isprint(string[i]) == 0) {
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_value_from_pointer(): detected an unprintable character");
                    errno = EINVAL;
                    return -1;
                }
            }

            //  If anything previously existed, free it
            if (dest_resource->value.string_v != NULL)
                free(dest_resource->value.string_v);

            dest_resource->value.string_v = strdup(string);

            if (dest_resource->value.string_v == NULL) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "out of memory!");
                return -1;
            }

            dest_resource->dirty = 1;
            return 0;
        }

        default:
            break;
    }

    // only reached if the data type is unknown
    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_value_from_pointer(): invalid data type %d passed in", dest_resource->data_type);
    errno = EINVAL;
    return -1;
}
#endif


int bionet_datapoint_value_from_string(bionet_datapoint_t *d, const char *value_string) {
    if (d == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_value_from_string(): NULL datapoint passed in!");
        errno = EINVAL;
        return -1;
    }

    if (d->resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_value_from_string(): passed-in datapoint has NULL Resource!");
        errno = EINVAL;
        return -1;
    }

    return bionet_datapoint_value_from_string_isolated(d->resource->data_type, &d->value, value_string);
}


int bionet_datapoint_value_from_string_isolated(bionet_resource_data_type_t data_type, bionet_datapoint_value_t *value, const char *value_string) {
    if (value_string == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_value_from_string_isolated(): NULL value string passed in!");
        errno = EINVAL;
        return -1;
    }

    if (value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_value_from_string_isolated(): NULL value passed in!");
        errno = EINVAL;
        return -1;
    }

    switch (data_type) {
        case BIONET_RESOURCE_DATA_TYPE_BINARY: {
            if (
                (strcasecmp(value_string, "on") == 0) ||
                (strcasecmp(value_string, "true") == 0) ||
                (strcasecmp(value_string, "yes") == 0) ||
                (strcasecmp(value_string, "1") == 0)
            ) {
                value->binary_v = 1;
                return 0;
            } else if (
                (strcasecmp(value_string, "off") == 0) ||
                (strcasecmp(value_string, "false") == 0) ||
                (strcasecmp(value_string, "no") == 0) ||
                (strcasecmp(value_string, "0") == 0)
            ) {
                value->binary_v = 0;
                return 0;
            } 
            
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error parsing Binary value from '%s'", value_string);
            errno = EBADMSG;
            return -1;
        }

        case BIONET_RESOURCE_DATA_TYPE_FLOAT:
            {
                char *end_ptr;
                float val;

                errno = 0;
                val = strtof(value_string, &end_ptr);
                if ((errno == 0) && (*value_string != '\0') && (*end_ptr == '\0') && (value_string != end_ptr)) {
                    value->float_v = val;
                    return 0;
                }
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "cannot parse Float value from '%s'", value_string);
                errno = EBADMSG;
                return -1;
            }

        case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
            {
                char *end_ptr;
                double val;

                errno = 0;
                val = strtod(value_string, &end_ptr);
                if ((errno == 0) && (*value_string != '\0') && (*end_ptr == '\0') && (value_string != end_ptr)) {
                    value->double_v = val;
                    return 0;
                }
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "cannot parse Double value from '%s'", value_string);
                errno = EBADMSG;
                return -1;
            }

        case BIONET_RESOURCE_DATA_TYPE_INT32:
            {
                char *end_ptr;
                int64_t val;

                val = strtoll(value_string, &end_ptr, 0);
                if ((val <= INT32_MAX) && (val >= INT32_MIN)) {
                    value->int32_v = (int32_t)val;
                    if ((*value_string != '\0') && (*end_ptr == '\0')) {
                        return 0;
                    }
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "cannot parse Int32 value from '%s', junk at the end of the string", value_string);
                } else {
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "cannot parse Int32 value from '%s', value out of range", value_string);
                }
                errno = EBADMSG;
                return -1;
            }


        case BIONET_RESOURCE_DATA_TYPE_UINT32:
            {
                char *end_ptr;
                uint64_t val;

                val = strtoull(value_string, &end_ptr, 0);
                if (val <= UINT32_MAX) {
                    value->uint32_v = (uint32_t)val;
                    if ((*value_string != '\0') && (*end_ptr == '\0')) {
                        return 0;
                    }
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "cannot parse UInt32 value from '%s', junk at the end of the string", value_string);
                } else {
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "cannot parse UInt32 value from '%s', value out of range", value_string);
                }
                errno = EBADMSG;
                return -1;
            }


        case BIONET_RESOURCE_DATA_TYPE_INT16:
            {
                char *end_ptr;
                long val;

                val = strtol(value_string, &end_ptr, 0);
                if ((val <= INT16_MAX) && (val >= INT16_MIN)) {
                    value->int16_v = (int16_t)val;
                    if ((*value_string != '\0') && (*end_ptr == '\0')) {
                        return 0;
                    }
                } 
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "cannot parse Int16 value from '%s'", value_string);
                errno = EBADMSG;
                return -1;
            }


        case BIONET_RESOURCE_DATA_TYPE_UINT16:
            {
                char *end_ptr;
                unsigned long val;

                val = strtoul(value_string, &end_ptr, 0);
                if (val <= UINT16_MAX) {
                    value->uint16_v = (uint16_t)val;
                    if ((*value_string != '\0') && (*end_ptr == '\0')) {
                        return 0;
                    }
                } 
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "cannot parse UInt16 value from '%s'", value_string);
                errno = EBADMSG;
                return -1;
            }


        case BIONET_RESOURCE_DATA_TYPE_INT8:
            {
                char *end_ptr;
                long val;

                val = strtol(value_string, &end_ptr, 0);
                if ((val <= INT8_MAX) && (val >= INT8_MIN)) {
                    value->int8_v = (int8_t)val;
                    if ((*value_string != '\0') && (*end_ptr == '\0')) {
                        return 0;
                    }
                } 
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "cannot parse Int8 value from '%s'", value_string);
                errno = EBADMSG;
                return -1;
            }


        case BIONET_RESOURCE_DATA_TYPE_UINT8:
            {
                char *end_ptr;
                unsigned long val;

                val = strtoul(value_string, &end_ptr, 0);
                if (val <= UINT8_MAX) {
                    value->uint8_v = (uint8_t)val;
                    if ((*value_string != '\0') && (*end_ptr == '\0')) {
                        return 0;
                    }
                } 
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "cannot parse UInt8 value from '%s'", value_string);
                errno = EBADMSG;
                return -1;
            }

        case BIONET_RESOURCE_DATA_TYPE_STRING:
            {
                int i;

                for (i = 0; value_string[i] != '\0'; i++) {
                    if (isprint(value_string[i]) == 0) {
                        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_string_to_value_isolated(): detected an unprintable character");
                        errno = EBADMSG;
                        return -1;
                    }
                }

                //  If anything previously existed, free it
                if (value->string_v != NULL)
                    free(value->string_v);
                
                value->string_v = strdup(value_string);

                if (value->string_v == NULL) {
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "out of memory!");
                    return -1;
                }

                return 0;
            }

        default: {
            break;
        }
    }


    // only reached if the data type is unknown
    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_string_to_value_isolated(): invalid data-type %d", data_type);
    errno = EINVAL;
    return -1;
}


#if 0
int bionet_resource_value_encode(bionet_resource_t *resource, void **p, int *size) {
    static char val_buf[256];

    switch (resource->data_type) {

        case BIONET_RESOURCE_DATA_TYPE_BINARY: {
            val_buf[0] = resource->value.binary_v;
            *p = val_buf;
            *size = 1;
            return 0;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT8: {
            val_buf[0] = resource->value.uint8_v;
            *p = val_buf;
            *size = 1;
            return 0;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT8: {
            val_buf[0] = resource->value.int8_v;
            *p = val_buf;
            *size = 1;
            return 0;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT16: {
            *(uint16_t *)val_buf = g_htons(resource->value.uint16_v);
            *p = val_buf;
            *size = 2;
            return 0;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT16: {
            *(int16_t *)val_buf = g_htons(resource->value.int16_v);
            *p = val_buf;
            *size = 2;
            return 0;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT32: {
            *(uint32_t *)val_buf = g_htonl(resource->value.uint32_v);
            *p = val_buf;
            *size = 4;
            return 0;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT32: {
            *(int32_t *)val_buf = g_htonl(resource->value.int32_v);
            *p = val_buf;
            *size = 4;
            return 0;
        }

        case BIONET_RESOURCE_DATA_TYPE_FLOAT: {
            *size = sizeof(float);
            memcpy(val_buf, &resource->value.float_v, *size);
            *p = val_buf;
            return 0;
        }

        case BIONET_RESOURCE_DATA_TYPE_DOUBLE: {
            *size = sizeof(double);
            memcpy(val_buf, &resource->value.double_v, *size);
            *p = val_buf;
            return 0;
        }

        case BIONET_RESOURCE_DATA_TYPE_STRING: {
            *size = strlen(resource->value.string_v) + 1;
            *p = resource->value.string_v;
            return 0;
        }

        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_value_encode():  unknown data-type %d", resource->data_type);
            return -1;
        }
    }

    // not reached
    return -1;
}




int bionet_resource_value_decode(bionet_resource_t *resource, const void *p, int size) {

    // 
    // sanity checks
    //

    if (resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_value_decode(): NULL Resource passed in\n");
        return -1;
    }

    if (p == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_value_decode(): NULL input string\n");
        return -1;
    }


    switch (resource->data_type) {

        case BIONET_RESOURCE_DATA_TYPE_BINARY: {
            if (size < 1) return 0;
            resource->value.binary_v = *(uint8_t *)p;
            return 1;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT8: {
            if (size < 1) return 0;
            resource->value.uint8_v = *(uint8_t *)p;
            return 1;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT8: {
            if (size < 1) return 0;
            resource->value.int8_v = *(int8_t *)p;
            return 1;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT16: {
            if (size < 2) return 0;
            resource->value.uint16_v = g_ntohs(*(uint16_t *)p);
            return 2;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT16: {
            if (size < 2) return 0;
            resource->value.int16_v = g_ntohs(*(int16_t *)p);
            return 2;
        }

        case BIONET_RESOURCE_DATA_TYPE_UINT32: {
            if (size < 4) return 0;
            resource->value.uint32_v = g_ntohl(*(uint32_t *)p);
            return 4;
        }

        case BIONET_RESOURCE_DATA_TYPE_INT32: {
            if (size < 4) return 0;
            resource->value.int32_v = g_ntohl(*(int32_t *)p);
            return 4;
        }

        case BIONET_RESOURCE_DATA_TYPE_FLOAT: {
            if (size < sizeof(float)) return 0;
            resource->value.float_v = *(float *)p;
            return sizeof(float);
        }

        case BIONET_RESOURCE_DATA_TYPE_DOUBLE: {
            if (size < sizeof(double)) return 0;
            resource->value.double_v = *(double *)p;
            return sizeof(double);
        }

        case BIONET_RESOURCE_DATA_TYPE_STRING: {
            int i;
            char *string = (char*)p;
            char *tmp;

            if (size < 1) return 0;  // must have at least the NULL

            for (i = 0; (i < size) && (string[i] != '\0'); i++) {
                if (isprint(string[i]) == 0) {
                    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_value_decode(): attempted to decode unprintable character");
                    return 0;
                }
            }

            if (string[i] != '\0') {
                // end-of-buffer before end-of-string
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_value_decode(): the passed-in buffer does not contain a NUL-terminated string");
                return 0;
            }

            tmp = strdup(string);
            if (tmp == NULL) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "out of memory!");
                return -1;
            }

            if (resource->value.string_v != NULL) 
                free(resource->value.string_v);

            resource->value.string_v = tmp;

            return i+1;  // +1 for the NUL
        }

        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_value_decode():  unknown data-type %d", resource->data_type);
            return -1;
        }
    }

    // not reached
    return -1;
}




int bionet_resource_value_is_valid(const bionet_resource_t *resource, const char *proposed_value) {
    bionet_resource_value_t val;
    int r;

    memset(&val, '\0', sizeof(val));

    r = bionet_resource_value_from_string_isolated(proposed_value, resource->data_type, &val);
    if (r != 0) {
        errno = EINVAL;
        return 0;
    }

    if (resource->data_type == BIONET_RESOURCE_DATA_TYPE_STRING)
        free(val.string_v);

    return 1;
}
#endif


