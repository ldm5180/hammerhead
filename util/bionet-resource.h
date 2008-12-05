
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
//  bionet_datapoint_value_t should always be initialized (all-bits-zero is valid)
//

typedef union {
    int binary_v;

    uint8_t  uint8_v;
    int8_t   int8_v;

    uint16_t uint16_v;
    int16_t  int16_v;

    uint32_t uint32_v;
    int32_t  int32_v;

    float float_v;
    double double_v;

    char *string_v;
} bionet_datapoint_value_t;


struct bionet_datapoint {
    bionet_resource_t *resource;  // the resource that this datapoint belongs to

    bionet_datapoint_value_t value;
    struct timeval timestamp;

    int dirty;  // 1 if the datapoint has new information that hasnt been reported to Bionet, 0 if the datapoint has nothing new
};




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




/**
 * @brief Sets the value of a Resource.
 *
 * If the Resource has no Datapoint, one will be created.  Then the
 * passed-in value and timestamp are copied to the Resource's first
 * Datapoint, and the Datapoint is marked dirty.
 *
 * @param resource  The Resource to set.
 *
 * @param value The new Value.
 *
 * @param timestamp The new Timestamp (if NULL, the current time will be
 * used).
 *
 * @return 0 on success, -1 on failure.
 */

int bionet_resource_set(bionet_resource_t *resource, const bionet_datapoint_value_t *value, const struct timeval *timestamp);


int bionet_resource_set_with_valuestr(bionet_resource_t *resource, const char *value_str, const struct timeval *timestamp);




bionet_datapoint_t *bionet_resource_add_datapoint(bionet_resource_t *resource, const char *value_str, const struct timeval *timestamp);
void bionet_resource_add_existing_datapoint(bionet_resource_t *resource, bionet_datapoint_t *new_datapoint);

int bionet_resource_get_num_datapoints(const bionet_resource_t *resource);
bionet_datapoint_t *bionet_resource_get_datapoint_by_index(const bionet_resource_t *resource, unsigned int index);




/**
 * @brief Checks if a Resource matches a name specification.
 *
 * @note Also see the bionet_resource_matches_habtype_habid_nodeid_resourceid() function.
 *
 * @param resource The Resource to check.
 *
 * @param id The Resource-ID to check against.  id may be a regular
 *     Resource-ID, or it may be the wildcard "*" that matches all
 *     Resource-IDs.
 *
 * @return FALSE (zero) if the Resource does not match the ID, TRUE
 *     (non-zero) if it matches.
 */

int bionet_resource_matches_id(const bionet_resource_t *resource, const char *id);




/**
 * @brief Checks if a Resource matches a name specification.
 *
 * @note Also see the bionet_resource_matches_id() function.
 *
 * @param resource  The Resource to check.
 *
 * @param hab_type  The HAB-Type to check against.  It may be a regular
 *     HAB-Type string, or it may be the wildcard "*" that matches all
 *     HAB-Types.
 *
 * @param hab_id  The HAB-ID to check against.  It may be a regular HAB-ID
 *     string, or it may be the wildcard "*" that matches all HAB-IDs.
 *
 * @param node_id  The Node-ID to check against.  It may be a regular
 *     Node-ID string, or it may be the wildcard "*" that matches all
 *     Node-IDs.
 *
 * @param resource_id  The Resource-ID to check against.  It may be a
 *     regular Resource-ID string, or it may be the wildcard "*" that
 *     matches all Resource-IDs.
 *
 * @return FALSE (zero) if the Resource does not match the ID, TRUE
 *     (non-zero) if it matches.
 */

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




//
//       NAME:  bionet_resource_make_clean()
//
//   FUNCTION:  Makes a Resource clean, by making all its Datapoints clean.
//
//  ARGUMENTS:  The Resource to clean.
//
//    RETURNS:  Nothing.
//

void bionet_resource_make_clean(bionet_resource_t *resource);




//
//
//       NAME:  bionet_datapoint_new()
//
//   FUNCTION:  Allocates and initializes a new datapoint.  Does *not* add
//              it to the Resource's list of data points.
//
//  ARGUMENTS:  All passed-in strings are considered the property of the
//              caller.  The caller is free to overwrite or free the
//              strings on return from this function.
//
//              resource: The Resource that the new datapoint is for.
//
//              value: The value of the new datapoint.
//
//              value_str: The value of the new datapoint, as an ASCII string.
//
//              timestamp: The timestamp of the new datapoint (NULL means
//                  "now").
//
//    RETURNS:  The new Datapoint if all went well, NULL on error.
//
//

bionet_datapoint_t *bionet_datapoint_new(
    bionet_resource_t *resource,
    const bionet_datapoint_value_t *value,
    const struct timeval *timestamp
);

bionet_datapoint_t *bionet_datapoint_new_with_valuestr(
    bionet_resource_t *resource,
    const char *value_str,
    const struct timeval *timestamp
);




void bionet_datapoint_set_value(bionet_datapoint_t *d, const bionet_datapoint_value_t *value);
int bionet_datapoint_value_from_string(bionet_datapoint_t *d, const char *value_string);




/**
 * @brief Sets a bionet_datapoint_value_t variable from a string.
 *
 * @param data_type  The data type to interpret the string as.
 *
 * @param value  A pointer to the variable to receive the value parsed from
 *     the string.  value must point to a valid, initialized
 *     bionet_datapoint_value_t variable (all bits zero is valid).
 *
 * @param value_string  The string containing the new value.
 *
 * @return 0 on success, -1 on error.
 */

int bionet_datapoint_value_from_string_isolated(bionet_resource_data_type_t data_type, bionet_datapoint_value_t *value, const char *value_string);




//
//       NAME:  bionet_datapoint_value_to_string()
//
//   FUNCTION:  Renders a Datapoint's Value as an ASCII string.
//
//  ARGUMENTS:  The Datapoint to get the Value from.
//
//    RETURNS:  A statically allocated string containing an ASCII
//              representation of the Datapoint Value, or NULL on error.
//

const char *bionet_datapoint_value_to_string(const bionet_datapoint_t *datapoint);
const char *bionet_datapoint_value_to_string_isolated(bionet_resource_data_type_t data_type, const bionet_datapoint_value_t *value);




//
//       NAME:  bionet_datapoint_timestamp_to_string()
//
//   FUNCTION:  Renders the Datapoint Timestamp as an ASCII string of the
//              form "YYYY-MM-DD HH:MM:SS.microseconds".
//
//  ARGUMENTS:  The Datapoint to get the Timestamp of.
//
//    RETURNS:  A pointer to the statically allocated string on success,
//              NULL on failure.
//

const char *bionet_datapoint_timestamp_to_string(const bionet_datapoint_t *datapoint);




//
//       NAME:  bionet_datapoint_set_timestamp()
//
//   FUNCTION:  Sets the Datapoint Timestamp as specified.
//
//  ARGUMENTS:  The Datapoint to set the Timestamp of, and the timestamp to
//              set it to (NULL means "now").
//
//    RETURNS:  Nothing.
//

void bionet_datapoint_set_timestamp(bionet_datapoint_t *datapoint, const struct timeval *new_timestamp);




//
//       NAME:  bionet_datapoint_is_dirty()
//
//   FUNCTION:  Checks if a Datapoint is dirty or not.  A Datapoint is
//              dirty if it has not been reported to Bionet yet.
//
//  ARGUMENTS:  The Datapoint to check.
//
//    RETURNS:  True (non-zero) if the Datapoint is dirty, False (zero) if
//              not.
//
//

int bionet_datapoint_is_dirty(const bionet_datapoint_t *datapoint);




//
//       NAME:  bionet_datapoint_make_clean()
//
//   FUNCTION:  Makes a Datapoint clean.
//
//  ARGUMENTS:  The Datapoint to make clean.
//
//    RETURNS:  Nothing.
//
//

void bionet_datapoint_make_clean(bionet_datapoint_t *datapoint);




//
//       NAME:  bionet_datapoint_free()
//
//   FUNCTION:  Frees a Datapoint created with bionet_datapoint_new().
//
//  ARGUMENTS:  The Datapoint to free.
//
//    RETURNS:  Nothing.
//
//

void bionet_datapoint_free(bionet_datapoint_t *datapoint);




#endif // __BIONET_RESOURCE_H


