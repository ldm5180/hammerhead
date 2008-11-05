
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef __BIONET_RESOURCE_H
#define __BIONET_RESOURCE_H




#include <stdint.h>
#include <sys/time.h>

#include <glib.h>

#include "bionet-util.h"




typedef enum {
    BIONET_RESOURCE_FLAVOR_INVALID = -1,
    BIONET_RESOURCE_FLAVOR_SENSOR,
    BIONET_RESOURCE_FLAVOR_ACTUATOR,
    BIONET_RESOURCE_FLAVOR_PARAMETER
} bionet_resource_flavor_t;

#define  BIONET_RESOURCE_FLAVOR_MIN  BIONET_RESOURCE_FLAVOR_SENSOR
#define  BIONET_RESOURCE_FLAVOR_MAX  BIONET_RESOURCE_FLAVOR_PARAMETER




typedef enum {
    BIONET_RESOURCE_DATA_TYPE_INVALID = -1,
    BIONET_RESOURCE_DATA_TYPE_BINARY,
    BIONET_RESOURCE_DATA_TYPE_UINT8,
    BIONET_RESOURCE_DATA_TYPE_INT8,
    BIONET_RESOURCE_DATA_TYPE_UINT16,
    BIONET_RESOURCE_DATA_TYPE_INT16,
    BIONET_RESOURCE_DATA_TYPE_UINT32,
    BIONET_RESOURCE_DATA_TYPE_INT32,
    BIONET_RESOURCE_DATA_TYPE_FLOAT,
    BIONET_RESOURCE_DATA_TYPE_DOUBLE,
    BIONET_RESOURCE_DATA_TYPE_STRING
} bionet_resource_data_type_t;

#define  BIONET_RESOURCE_DATA_TYPE_MIN  BIONET_RESOURCE_DATA_TYPE_BINARY
#define  BIONET_RESOURCE_DATA_TYPE_MAX  BIONET_RESOURCE_DATA_TYPE_STRING




// 
// This holds a resource.  'flavor', 'id', and 'data_type' are all used by
// the system.  'user_data' is for the caller to do with as it pleases.
//

struct bionet_resource {
    const bionet_node_t *node;

    char *id;

    bionet_resource_flavor_t flavor;
    bionet_resource_data_type_t data_type;

    GPtrArray *datapoints;

    void *user_data;
};




//
//
//       NAME:  bionet_resource_new()
//
//   FUNCTION:  Allocates and initializes a new resource, from strings
//              describing the resource.
//
//  ARGUMENTS:  All passed-in strings are considered the property of the
//              caller.  The function duplicates what it needs, the caller
//              is free to overwrite or free the strings on return from
//              this function.
//
//              node: The Node that owns this Resource.  (optional)
//
//              data_type_str: The name of the data type of this Resource.
//
//              flavor_str: The flavor of this Resource.
//
//              id: The ID of this Resource.
//
//    RETURNS:  The Resource if all went well, NULL on error.
//
//

bionet_resource_t *bionet_resource_new(
    const bionet_node_t *node,
    bionet_resource_data_type_t data_type,
    bionet_resource_flavor_t flavor,
    const char *id
);

bionet_resource_t *bionet_resource_new_from_str(
    const bionet_node_t *node,
    const char *data_type_str,
    const char *flavor_str,
    const char *id
);




//
//
//       NAME:  bionet_resource_free()
//
//   FUNCTION:  Frees a Resource created with bionet_resource_new().
//
//  ARGUMENTS:  The Resource to free.
//
//    RETURNS:  Nothing.
//
//

void bionet_resource_free(bionet_resource_t *resource);




//
//       NAME:  bionet_resource_flavor_to_string()
//
//   FUNCTION:  Renders a Resource-Flavor as an ASCII string.  The output
//              will be one of "Sensor", "Actuator", or "Parameter".
//
//  ARGUMENTS:  The Resource Flavor to render.
//
//    RETURNS:  A pointer to the statically allocated string on success,
//              NULL on error.
//

const char *bionet_resource_flavor_to_string(bionet_resource_flavor_t flavor);




//
//       NAME:  bionet_resource_flavor_from_string()
//
//   FUNCTION:  Tries to parse an ASCII string as a Resource-Flavor, and
//              returns the Resource-Flavor.  Supported Resource-Flavors are
//              "sensor", "actuator", and "parameter".  Case insensitive.
//
//  ARGUMENTS:  The string to parse the Resource-Flavor from.
//
//    RETURNS:  The Resource-Flavor (which may be
//              BIONET_RESOURCE_FLAVOR_INVALID to indicate error).
//

bionet_resource_flavor_t bionet_resource_flavor_from_string(const char *flavor_string);




//
//       NAME:  bionet_resource_data_type_to_string()
//
//   FUNCTION:  Renders a Resource-Data-Type as an ASCII string.
//
//  ARGUMENTS:  The Resource-Data-Type to render.
//
//    RETURNS:  A pointer to the statically allocated string on success,
//              NULL on error.
//

const char *bionet_resource_data_type_to_string(bionet_resource_data_type_t data_type);




//
//       NAME:  bionet_resource_data_type_from_string()
//
//   FUNCTION:  Tries to parse an ASCII string as a Resource-Data-Type, and
//              returns the Resource-Data-Type.  Case insensitive.
//
//  ARGUMENTS:  The string to parse the Resource-Flavor from.
//
//    RETURNS:  The Resource-Flavor (which may be
//              BIONET_RESOURCE_FLAVOR_INVALID to indicate error).
//

bionet_resource_data_type_t bionet_resource_data_type_from_string(const char *data_type_string);




bionet_datapoint_t *bionet_resource_add_datapoint(bionet_resource_t *resource, const char *value_str, const struct timeval *timestamp);
void bionet_resource_add_existing_datapoint(bionet_resource_t *resource, bionet_datapoint_t *new_datapoint);

int bionet_resource_get_num_datapoints(const bionet_resource_t *resource);
bionet_datapoint_t *bionet_resource_get_datapoint_by_index(const bionet_resource_t *resource, unsigned int index);




//
//       NAME:  bionet_resource_matches_id()
//              bionet_resource_matches_habtype_habid_nodeid_resourceid()
//
//   FUNCTION:  Checks if a Resource matches a name specification.  The
//              Resource name can be specified as just a Resource-ID (in
//              which case the Resource's HAB-Type, HAB-ID, and Node-ID are
//              considered matching no matter what), or as a HAB-Type,
//              HAB-ID, Node-ID, and Resource-ID (in which case all must
//              match).  The wildcard "*" matches all strings.
//
//  ARGUMENTS:  The Resource to test for match, and the relevant Resource
//              name components.
//
//    RETURNS:  TRUE (1) if it matches, FALSE (0) if not.
//

int bionet_resource_matches_id(const bionet_resource_t *resource, const char *id);
int bionet_resource_matches_habtype_habid_nodeid_resourceid(
    const bionet_resource_t *resource,
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id
);




//
//       NAME:  bionet_resource_is_dirty()
//
//   FUNCTION:  Checks if a Resource has any dirty Datapoints.
//              Dirty Datapoints are ones that haven't been reported to
//              Bionet yet.
//
//  ARGUMENTS:  The Resource to test for dirtiness.
//
//    RETURNS:  True (non-zero) if it is dirty, False (zero) if not.
//

int bionet_resource_is_dirty(const bionet_resource_t *resource);




#if 0

//
//
//       NAME:  bionet_resource_value_is_valid()
//
//   FUNCTION:  Given a string and a Resource, verifies that the string
//              contains a valid Resource Value for that Resource.
//
//  ARGUMENTS:  The Resource and a string containing the proposed value.
//
//    RETURNS:  On success it returns TRUE (1).  On failure it sets errno to
//              EINVAL and returns FALSE (0).
//
//

int bionet_resource_value_is_valid(const bionet_resource_t *resource, const char *proposed_value);




//
//       NAME:  bionet_resource_value_to_string_isolated()
//
//   FUNCTION:  Renders a Resource-Value as an ASCII string, without the
//              Resource-Value being wrapped up in a Resource.
//
//  ARGUMENTS:  The Resource-Value and Resource-Data-Type to render.
//
//    RETURNS:  A statically allocated string containing an ASCII
//              representation of the Resource-Value, or NULL on error.
//

const char *bionet_resource_value_to_string_isolated(bionet_resource_data_type_t data_type, const bionet_resource_value_t *value);




//
//       NAME:  bionet_resource_value_from_string()
//   
//   FUNCTION:  Sets a Resource's Value from an ASCII string
//              representation.
//
//  ARGUMENTS:  The string to set the Value from, and the Resource to set
//              the Value on.
//
//    RETURNS:  On success it returns 0.  On failure it returns -1 and sets
//              errno:
//                  EINVAL: invalid argument passed in
//
//              The errnos from bionet_resource_value_from_string_isolated()
//              may also be returned.
//

int bionet_resource_value_from_string(const char *value_string, bionet_resource_t *dest_resource);




//
//       NAME:  bionet_resource_value_from_string_isolated()
//   
//   FUNCTION:  Sets a Resource-Value from an ASCII string representation,
//              without the Value being wrapped in a full Resource.
//
//  ARGUMENTS:  The string to set the Value from, and the
//              Resource-Data-Type and Resource-Value to set.
//
//    RETURNS:  On success it returns 0.  On failure it returns -1 and sets
//              errno:
//                  EINVAL: invalid argument passed in
//                  EBADMSG: failed to parse value from string
//

int bionet_resource_value_from_string_isolated(const char *value_string, bionet_resource_data_type_t data_type, bionet_resource_value_t *value);




//
//       NAME:  bionet_resource_value_encode()
//
//   FUNCTION:  Creates a compact binary encoding of the resource's value,
//              and returns a pointer to it (and the size of the encoding).
//
//  ARGUMENTS:  The Resource to encode, a pointer to void* to hold the
//              encoding, and a pointer to int to hold the encoded size.
//              The encoding buffer is owned by this function, and must not
//              be modified or freed by the caller.  The contents of the
//              buffer will be stable until the next call to this function.
//              NOTE: string values are NOT stable (p is a pointer to the
//              resource value string, which can change).
//
//    RETURNS:  On success returns 0.  On failure returns -1, and *p and
//              *size are undefined.
//

int bionet_resource_value_encode(bionet_resource_t *resource, void **p, int *size);




//
//       NAME:  bionet_resource_value_decode()
//
//   FUNCTION:  Updates a resource's value from a compact binary encoding
//              (as produced by bionet_resource_value_encode()).
//
//  ARGUMENTS:  The Resource to decode, a pointer to the buffer holding the
//              encoded value, and the buffer's size.
//
//    RETURNS:  On success returns the number of bytes consumed out of the
//              buffer.
//
//              Returns 0 if the buffer is too small, or did not contain a
//              valid encoded Resource Value of the Resource's Data Type.
//              In this case no decoding was accomplished, no bytes were
//              consumed from the buffer, and the Resource Value is
//              unchanged.
//
//              Returns -1 if the arguments are invalid or some other
//              environmental (ie, not data-related) problem occurs.
//              The Resource Value is unchanged.
//

int bionet_resource_value_decode(bionet_resource_t *resource, const void *p, int size);




//
//       NAME:  bionet_resource_value_from_pointer()
//   
//   FUNCTION:  Sets a Resource's Value from a void pointer pointing at the
//              appropriate value.
//
//  ARGUMENTS:  The void pointer to set the Value from, and the Resource to
//              set the Value on.
//
//    RETURNS:  On success, it returns 0.  On failure, it returns -1 and
//              sets errno:
//                  EINVAL: invalid argument passed in
//
int bionet_resource_value_from_pointer(const void *value, bionet_resource_t *dest_resource);




//
//       NAME:  bionet_resource_time_to_string()
//              bionet_resource_time_to_string_human_readable()
//
//   FUNCTION:  Renders the Resource Timestamp as an ASCII string.  The
//              first function outputs a string of the form
//              "<seconds>.<microseconds>", where <seconds> is the number
//              of seconds since 00:00:00 UTC, January 1, 1970.  The
//              _human_readable() function outputs a string of the form
//              "YYYY-MM-DD HH:MM:SS.microseconds", intended for human
//              consumption.
//
//  ARGUMENTS:  The Resource to get the Timestamp of.
//
//    RETURNS:  A pointer to the statically allocated string on success,
//              NULL on failure.
//

const char *bionet_resource_time_to_string(const bionet_resource_t *resource);
const char *bionet_resource_time_to_string_human_readable(const bionet_resource_t *resource);




//
//       NAME:  bionet_resource_time_from_string()
//              bionet_resource_time_from_timeval()
//              bionet_resource_time_now()
//
//  ARGUMENTS:  The Resource to set the Timestamp of, and optionally the
//              timestamp.  The new timestamp can be submitted as an ASCII
//              string (formatted like the output of the
//              bionet_resource_time_to_string*() functions) or as a struct
//              timeval (as returned, for example, from gettimeofday()), or
//              it can be omitted to indicate "now".  If the passed-in
//              string or timeval is NULL, "now" will be used.
//
//    RETURNS:  On success it returns 0.  On failure it returns -1 and sets
//              errno:
//
//                  EINVAL: invalid argument passed in
//
//                  EBADMSG: bionet_resource_time_from_string() failed to
//                      parse passed-in time string
//
//              The errnos from gettimeofday() may also be returned on
//              failure.
//

int bionet_resource_time_from_string(const char *time_str, bionet_resource_t *dest_resource);
int bionet_resource_time_from_timeval(const struct timeval *tv, bionet_resource_t *dest_resource);
int bionet_resource_time_now(bionet_resource_t *resource);




//
//       NAME:  bionet_resource_time_encode()
//
//   FUNCTION:  Creates a compact binary encoding of the resource's time
//              stamp, and returns a pointer to it (and the size of the
//              encoding).
//
//  ARGUMENTS:  The Resource to encode, a pointer to void* to hold the
//              encoding, and a pointer to int to hold the encoded size.
//              The encoding buffer is owned by this function, and must not
//              be modified or freed by the caller.  The contents of the
//              buffer will be stable until the next call to this function.
//
//    RETURNS:  On success returns 0.  On error returns -1, and *p and *size
//              are undefined.
//

int bionet_resource_time_encode(const bionet_resource_t *resource, void **p, int *size);




//
//       NAME:  bionet_resource_time_decode()
//
//   FUNCTION:  Updates a resource's timestamp from a compact binary
//              encoding (as produced by bionet_resource_time_encode()).
//
//  ARGUMENTS:  The Resource to decode, a pointer to the buffer holding the
//              encoded timestamp, and the buffer's size.
//
//    RETURNS:  On success return 0.  On error returns -1, and the Resource
//              timestamp is undefined.
//

int bionet_resource_time_decode(bionet_resource_t *resource, const void *p, int size);

#endif




#endif // __BIONET_RESOURCE_H


