
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef __BIONET_RESOURCE_H
#define __BIONET_RESOURCE_H

/**
 * @file bionet-resource.h
 * Functions for dealing with Bionet Resources.
 */


#include <stdint.h>
#include <sys/time.h>

#include <glib.h>

#include "bionet-util.h"



/**
 * @typedef bionet_resource_flavor_t
 * See the value in quotes for the string conversion value
 */
typedef enum {
    BIONET_RESOURCE_FLAVOR_INVALID = -1, /**< "Invalid" */
    BIONET_RESOURCE_FLAVOR_SENSOR, /**< "Sensor" */
    BIONET_RESOURCE_FLAVOR_ACTUATOR, /**< "Actuator" */
    BIONET_RESOURCE_FLAVOR_PARAMETER /**< "Parameter" */
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




/**
 * @union bionet_datapoint_value_t
 * @brief Union to describe how to read a value 
 *
 * @note bionet_datapoint_value_t should always be initialized 
 * (all-bits-zero is valid)
 */
typedef union {
    int binary_v; /**< 0 or 1, TRUE or FALSE */

    uint8_t  uint8_v; /**< 8-bit unsigned integer */
    int8_t   int8_v; /**< 8-bit signed integer */

    uint16_t uint16_v; /**< 16-bit unsigned integer */
    int16_t  int16_v; /**< 16-bit signed integer */

    uint32_t uint32_v; /**< 32-bit unsigned integer */
    int32_t  int32_v; /**< 32-bit signed integer */

    float float_v; /**< float */
    double double_v; /**< double */

    char *string_v; /**< NULL-terminated array of characters */
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


/**
 * @brief Allocates and initializes a new resource
 *
 * @param[in] node The Node that owns this Resource or NULL
 * @param[in] data_type Data type of this Resource.
 * @param[in] flavor The flavor of this Resource.
 * @param[in] id The ID of this Resource.
 *
 * @return Pointer to a resource on success
 * @retval NULL Error
 * @retval >0 Success
 *
 * @note All passed-in strings are considered the property of the
 *       caller.  The function duplicates what it needs, the caller
 *       is free to overwrite or free the strings on return from
 *       this function.
 */
bionet_resource_t *bionet_resource_new(
    const bionet_node_t *node,
    bionet_resource_data_type_t data_type,
    bionet_resource_flavor_t flavor,
    const char *id
);


/**
 * @brief Allocates and initializes a new resource, from strings
 *        describing the resource.
 *
 * @param[in] node The Node that owns this Resource or NULL
 * @param[in] data_type_str The name of the data type of this Resource as a string.
 * @param[in] flavor_str The flavor of this Resource as a string.
 * @param[in] id The ID of this Resource.
 *
 * @return Pointer to a resource on success
 * @retval NULL Error
 * @retval >0 Success
 *
 * @note All passed-in strings are considered the property of the
 *       caller.  The function duplicates what it needs, the caller
 *       is free to overwrite or free the strings on return from
 *       this function.
 */
bionet_resource_t *bionet_resource_new_from_str(
    const bionet_node_t *node,
    const char *data_type_str,
    const char *flavor_str,
    const char *id
);


/**
 * @brief Free a Resource created with bionet_resource_new().
 *
 * @param[in] resource The Resource to free.
 */
void bionet_resource_free(bionet_resource_t *resource);


/**
 * @brief Renders a Resource-Flavor as an ASCII string.
 *
 * See @ref bionet_resource_flavor_t for the string values
 *
 * @param[in] flavor The Resource Flavor to render.
 *
 * @return A pointer to the statically allocated string on success
 * @retval NULL Error
 */
const char *bionet_resource_flavor_to_string(bionet_resource_flavor_t flavor);


/**
 * @brief Tries to parse an ASCII string as a Resource-Flavor, and
 *        returns the Resource-Flavor. See @ref bionet_resource_flavor_t for 
 *        supported Resource-Flavors
 *
 * @param[in] flavor_string The string to parse the Resource-Flavor from.
 *
 * @return The Resource-Flavor
 * @retval #BIONET_RESOURCE_FLAVOR_INVALID Error
 *
 * @note flavor_string is case-insensitive
 */
bionet_resource_flavor_t bionet_resource_flavor_from_string(const char *flavor_string);


/**
 * @brief Renders a Resource-Data-Type as an ASCII string.
 *
 * See @ref bionet_resource_data_type_t for the string values
 *
 * @param[in] data_type The Resource-Data-Type to render.
 *
 * @return A pointer to the statically allocated string on success,
 * @retval NULL Error
 */
const char *bionet_resource_data_type_to_string(bionet_resource_data_type_t data_type);


/**
 * @brief Tries to parse an ASCII string as a Resource-Data-Type, and
 *        returns the Resource-Data-Type. See @ref bionet_resource_data_type_t 
 *        for supported Resource-Data-Types
 *
 * @param[in] data_type_string The string to parse the Resource-Flavor from.
 *
 * @return The Resource-Flavor
 * @retval #BIONET_RESOURCE_FLAVOR_INVALID Error
 *
 * @note data_type_string is case-insensitive
 */
bionet_resource_data_type_t bionet_resource_data_type_from_string(const char *data_type_string);


/**
 * @brief Sets the value of a Resource.
 *
 * If the Resource has no Datapoint, one will be created.  Then the
 * passed-in value and timestamp are copied to the Resource's first
 * Datapoint, and the Datapoint is marked dirty.
 *
 * @param[in] resource The Resource to set.
 * @param[in] value The new Value.
 * @param[in] timestamp The Timestamp, if NULL the current time will be used
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
int bionet_resource_set(bionet_resource_t *resource, 
			const bionet_datapoint_value_t *value, 
			const struct timeval *timestamp);


/**
 * @brief Sets the value of a Resource from a string
 *
 * If the Resource has no Datapoint, one will be created.  Then the
 * passed-in value and timestamp are copied to the Resource's first
 * Datapoint, and the Datapoint is marked dirty.
 *
 * @param[in] resource The Resource to set.
 * @param[in] value_str The new Value.
 * @param[in] timestamp The Timestamp, if NULL the current time will be used
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
int bionet_resource_set_with_valuestr(bionet_resource_t *resource, 
				      const char *value_str, 
				      const struct timeval *timestamp);


/**
 * @brief Add a datapoint to a resource from a value string
 *
 * @param[in] resource The resource to which the datapoint is added
 * @param[in] value_str The value for the datapoint
 * @param[in] timestamp The Timestamp, if NULL the current time will be used
 *
 * @return Pointer to the new datapoint
 * @retval NULL Failure
 */
bionet_datapoint_t *bionet_resource_add_datapoint(bionet_resource_t *resource, 
						  const char *value_str, 
						  const struct timeval *timestamp);



/**
 * @brief Add a datapoint to a resource
 *
 * @param[in] resource The resource to which the datapoint is added
 * @param[in] new_datapoint The datapoint to add
 */
void bionet_resource_add_existing_datapoint(bionet_resource_t *resource, 
					    bionet_datapoint_t *new_datapoint);


/**
 * @brief Get the number of datapoints in a resource
 *
 * @param[in] resource The resource
 *
 * @return The number of datapoints in a resource
 * @retval -1 Invalid resource
 */
int bionet_resource_get_num_datapoints(const bionet_resource_t *resource);


/**
 * @brief Get a datapoint by index in the resource's list
 *
 * This can be used to iterate over a list of datapoints.
 *
 * @param[in] resource The resource
 * @param[in] index Index of requested datapoint
 *
 * @return Pointer to a datapoint
 * @retval NULL Failure
 */
bionet_datapoint_t *bionet_resource_get_datapoint_by_index(const bionet_resource_t *resource, unsigned int index);


/**
 * @brief Checks if a Resource matches a name specification.
 *
 * @see bionet_resource_matches_habtype_habid_nodeid_resourceid()
 *
 * @param[in] resource The Resource to check.
 * @param[in] id The Resource-ID to check against.
 *
 * @note Any string may be replaced by the wildcard "*" to match all
 *
 * @retval 0 FALSE - The Resource does not match the ID
 * @retval 1 TRUE - The Resource does match the ID
 */
int bionet_resource_matches_id(const bionet_resource_t *resource, 
			       const char *id);


/**
 * @brief Checks if a Resource matches a name specification.
 *
 * @see bionet_resource_matches_id()
 *
 * @param[in] resource The Resource to check.
 * @param[in] hab_type The HAB-Type to check against. 
 * @param[in] hab_id  The HAB-ID to check against.
 * @param[in] node_id  The Node-ID to check against.
 * @param[in] resource_id  The Resource-ID to check against.
 *
 * @note Any string may be replaced by the wildcard "*" to match all
 *
 * @retval 0 FALSE - The Resource does not match the ID
 * @retval 1 TRUE - The Resource does match the ID
 */
int bionet_resource_matches_habtype_habid_nodeid_resourceid(
    const bionet_resource_t *resource,
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id
);


/**
 * @brief Checks if a Resource has any dirty Datapoints.
 *
 * Dirty Datapoints are ones that have data which hasn't been reported to
 * Bionet yet.
 *
 * @param[in] resource The Resource to test for dirtiness.
 *
 * @retval True (non-zero) - Dirty
 * @retval False (zero) - Not dirty
 */
int bionet_resource_is_dirty(const bionet_resource_t *resource);


/**
 * @brief Makes a Resource clean, by making all its Datapoints clean.
 *
 * @param[in] resource The Resource to clean.
 */
void bionet_resource_make_clean(bionet_resource_t *resource);


/**
 * @brief Allocates and initializes a new datapoint.  
 *
 * Does NOT add it to the Resource's list of data points.
 *
 * @param[in] resource The Resource that the new datapoint is for
 * @param[in] value The value of the new datapoint
 * @param[in] timestamp The timestamp of the new datapoint (NULL means "now")
 *
 * @return The new Datapoint 
 * @retval NULL Error
 *
 * @note All passed-in strings are considered the property of the
 *       caller.  The caller is free to overwrite or free the
 *       strings on return from this function.
 */
bionet_datapoint_t *bionet_datapoint_new(
    bionet_resource_t *resource,
    const bionet_datapoint_value_t *value,
    const struct timeval *timestamp
);

/**
 * @brief Allocates and initializes a new datapoint.  
 *
 * Does NOT add it to the Resource's list of data points.
 *
 * @param[in] resource The Resource that the new datapoint is for
 * @param[in] value_str The value of the new datapoint, as an ASCII string.
 * @param[in] timestamp The timestamp of the new datapoint (NULL means "now")
 *
 * @return The new Datapoint 
 * @retval NULL Failure
 *
 * @note All passed-in strings are considered the property of the
 *       caller.  The caller is free to overwrite or free the
 *       strings on return from this function.
 */
bionet_datapoint_t *bionet_datapoint_new_with_valuestr(
    bionet_resource_t *resource,
    const char *value_str,
    const struct timeval *timestamp
);


/**
 * @brief Set the value of a datapoint
 *
 * @param[in] d Datapoint to set
 * @param[in] value Value to set in the datapoint
 */
void bionet_datapoint_set_value(bionet_datapoint_t *d, 
				const bionet_datapoint_value_t *value);


/**
 * @brief Set the value of a datapoint
 *
 * @param[in] d Datapoint to set
 * @param[in] value_string Value to set in the datapoint, as an ASCII string
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
int bionet_datapoint_value_from_string(bionet_datapoint_t *d, 
				       const char *value_string);


/**
 * @brief Sets a bionet_datapoint_value_t variable from a string.
 *
 * @param[in] data_type The data type to interpret the string as.
 * @param[in] value A pointer to the variable to receive the value parsed from
 *     the string. value must point to a valid, initialized
 *     bionet_datapoint_value_t variable (all bits zero is valid).
 * @param[in] value_string The string containing the new value.
 *
 * @retval 0 Success
 * @retval -1 Failure
 */

int bionet_datapoint_value_from_string_isolated(bionet_resource_data_type_t data_type, 
						bionet_datapoint_value_t *value,
						const char *value_string);


/**
 * @brief Renders a Datapoint's Value as an ASCII string.
 *
 * @param[in] datapoint The Datapoint to get the Value from.
 *
 * @return A statically allocated string containing an ASCII
 *         representation of the Datapoint Value
 * @retval NULL Failure
 */
const char *bionet_datapoint_value_to_string(const bionet_datapoint_t *datapoint);


/**
 * @brief Renders a Value as an ASCII string.
 *
 * @param[in] data_type Datatype of the value
 * @param[in] value The Datapoint to get the Value from.
 *
 * @return A statically allocated string containing an ASCII
 *         representation of the Value
 * @retval NULL Failure
 */
const char *bionet_datapoint_value_to_string_isolated(bionet_resource_data_type_t data_type, const bionet_datapoint_value_t *value);


/**
 * @brief Renders the Datapoint Timestamp as an ASCII string of the
 *        form "YYYY-MM-DD HH:MM:SS.microseconds".
 *
 * @param[in] datapoint The Datapoint to get the Timestamp of.
 *
 * @return A pointer to the statically allocated string on success
 * @retval NULL Failure
 */
const char *bionet_datapoint_timestamp_to_string(const bionet_datapoint_t *datapoint);


/**
 * @brief Sets the Datapoint Timestamp as specified.
 *
 * @param[in] datapoint The Datapoint to set the Timestamp of
 * @param[in] new_timestamp Timestamp to set it to (NULL means "now").
 */
void bionet_datapoint_set_timestamp(bionet_datapoint_t *datapoint, 
				    const struct timeval *new_timestamp);


/**
 * @brief Checks if a Datapoint is dirty or not.  
 *
 * A Datapoint is dirty if it has not been reported to Bionet yet.
 *
 * @param[in] datapoint The Datapoint to check.
 *
 * @retval TRUE (non-zero) - Datapoint is dirty
 * @retval FALSE (zero) - Datapoint is not dirty
 */
int bionet_datapoint_is_dirty(const bionet_datapoint_t *datapoint);


/**
 * @brief Makes a Datapoint clean.
 *
 * @param datapoint The Datapoint to make clean.
 */
void bionet_datapoint_make_clean(bionet_datapoint_t *datapoint);


/**
 * @brief Frees a Datapoint created with bionet_datapoint_new()
 *
 * @param[in] datapoint The Datapoint to free.
 */
void bionet_datapoint_free(bionet_datapoint_t *datapoint);


#endif // __BIONET_RESOURCE_H


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
