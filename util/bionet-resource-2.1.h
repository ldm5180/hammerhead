
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

#include "bionet-util-2.1.h"



/**
 * @typedef bionet_resource_flavor_t
 * See the value in quotes for the string conversion value
 */
typedef enum {
    BIONET_RESOURCE_FLAVOR_INVALID = -1, /**< "Invalid" */
    BIONET_RESOURCE_FLAVOR_SENSOR,       /**< "Sensor" */
    BIONET_RESOURCE_FLAVOR_ACTUATOR,     /**< "Actuator" */
    BIONET_RESOURCE_FLAVOR_PARAMETER     /**< "Parameter" */
} bionet_resource_flavor_t;

#define  BIONET_RESOURCE_FLAVOR_MIN  BIONET_RESOURCE_FLAVOR_SENSOR
#define  BIONET_RESOURCE_FLAVOR_MAX  BIONET_RESOURCE_FLAVOR_PARAMETER




typedef enum {
    BIONET_RESOURCE_DATA_TYPE_INVALID = -1,
    BIONET_RESOURCE_DATA_TYPE_BINARY, /**< TRUE (1) or FALSE (0) */
    BIONET_RESOURCE_DATA_TYPE_UINT8,  /**< 8 bit unsigned integer */
    BIONET_RESOURCE_DATA_TYPE_INT8,   /**< 8 bit signed integer */
    BIONET_RESOURCE_DATA_TYPE_UINT16, /**< 16 bit unsigned integer */
    BIONET_RESOURCE_DATA_TYPE_INT16,  /**< 16 bit signed integer */
    BIONET_RESOURCE_DATA_TYPE_UINT32, /**< 32 bit unsigned integer */
    BIONET_RESOURCE_DATA_TYPE_INT32,  /**< 32 bit signed integer */
    BIONET_RESOURCE_DATA_TYPE_FLOAT,  /**< floating point number */
    BIONET_RESOURCE_DATA_TYPE_DOUBLE, /**< double */
    BIONET_RESOURCE_DATA_TYPE_STRING  /**< NULL terminated C-string */
} bionet_resource_data_type_t;

#define  BIONET_RESOURCE_DATA_TYPE_MIN  BIONET_RESOURCE_DATA_TYPE_BINARY
#define  BIONET_RESOURCE_DATA_TYPE_MAX  BIONET_RESOURCE_DATA_TYPE_STRING


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
    bionet_node_t *node,
    bionet_resource_data_type_t data_type,
    bionet_resource_flavor_t flavor,
    const char *id
);


/**
 * @brief Free a Resource created with bionet_resource_new().
 *
 * @param[in] resource The Resource to free.
 */
void bionet_resource_free(bionet_resource_t *resource);


/**
 * @brief Get ID of a resource
 *
 * @param[in] resource The resource
 *
 * @return ID of the resource
 * @return NULL Error
 */
const char *bionet_resource_get_id(bionet_resource_t *resource);


/**
 * @brief Get the node this resource belongs to
 *
 * @param[in] resource The Resource
 *
 * @return Pointer to Node
 * @return NULL Error
 */
bionet_node_t * bionet_resource_get_node(const bionet_resource_t *resource);


/**
 * @brief Get the data type of a resource
 *
 * @param[in] resource The resource
 *
 * @return Type of resource
 */
bionet_resource_data_type_t bionet_resource_get_data_type(const bionet_resource_t *resource);


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
 * @param[in] content The new content.
 * @param[in] timestamp The Timestamp, if NULL the current time will be used
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
int bionet_resource_set_binary(bionet_resource_t *resource, 
			       int content, 
			       const struct timeval *timestamp);


/**
 * @brief Sets the value of a Resource.
 *
 * If the Resource has no Datapoint, one will be created.  Then the
 * passed-in value and timestamp are copied to the Resource's first
 * Datapoint, and the Datapoint is marked dirty.
 *
 * @param[in] resource The Resource to set.
 * @param[in] content The new content.
 * @param[in] timestamp The Timestamp, if NULL the current time will be used
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
int bionet_resource_set_uint8(bionet_resource_t *resource, 
			      uint8_t content, 
			      const struct timeval *timestamp);


/**
 * @brief Sets the value of a Resource.
 *
 * If the Resource has no Datapoint, one will be created.  Then the
 * passed-in value and timestamp are copied to the Resource's first
 * Datapoint, and the Datapoint is marked dirty.
 *
 * @param[in] resource The Resource to set.
 * @param[in] content The new content.
 * @param[in] timestamp The Timestamp, if NULL the current time will be used
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
int bionet_resource_set_int8(bionet_resource_t *resource, 
			     int8_t content, 
			     const struct timeval *timestamp);


/**
 * @brief Sets the value of a Resource.
 *
 * If the Resource has no Datapoint, one will be created.  Then the
 * passed-in value and timestamp are copied to the Resource's first
 * Datapoint, and the Datapoint is marked dirty.
 *
 * @param[in] resource The Resource to set.
 * @param[in] content The new content.
 * @param[in] timestamp The Timestamp, if NULL the current time will be used
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
int bionet_resource_set_uint16(bionet_resource_t *resource, 
			       uint16_t content, 
			       const struct timeval *timestamp);


/**
 * @brief Sets the value of a Resource.
 *
 * If the Resource has no Datapoint, one will be created.  Then the
 * passed-in value and timestamp are copied to the Resource's first
 * Datapoint, and the Datapoint is marked dirty.
 *
 * @param[in] resource The Resource to set.
 * @param[in] content The new content.
 * @param[in] timestamp The Timestamp, if NULL the current time will be used
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
int bionet_resource_set_int16(bionet_resource_t *resource, 
			      int16_t content, 
			      const struct timeval *timestamp);


/**
 * @brief Sets the value of a Resource.
 *
 * If the Resource has no Datapoint, one will be created.  Then the
 * passed-in value and timestamp are copied to the Resource's first
 * Datapoint, and the Datapoint is marked dirty.
 *
 * @param[in] resource The Resource to set.
 * @param[in] content The new content.
 * @param[in] timestamp The Timestamp, if NULL the current time will be used
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
int bionet_resource_set_uint32(bionet_resource_t *resource, 
			       uint32_t content, 
			       const struct timeval *timestamp);


/**
 * @brief Sets the value of a Resource.
 *
 * If the Resource has no Datapoint, one will be created.  Then the
 * passed-in value and timestamp are copied to the Resource's first
 * Datapoint, and the Datapoint is marked dirty.
 *
 * @param[in] resource The Resource to set.
 * @param[in] content The new content.
 * @param[in] timestamp The Timestamp, if NULL the current time will be used
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
int bionet_resource_set_int32(bionet_resource_t *resource, 
			      int32_t content, 
			      const struct timeval *timestamp);


/**
 * @brief Sets the value of a Resource.
 *
 * If the Resource has no Datapoint, one will be created.  Then the
 * passed-in value and timestamp are copied to the Resource's first
 * Datapoint, and the Datapoint is marked dirty.
 *
 * @param[in] resource The Resource to set.
 * @param[in] content The new content.
 * @param[in] timestamp The Timestamp, if NULL the current time will be used
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
int bionet_resource_set_float(bionet_resource_t *resource, 
			      float content, 
			      const struct timeval *timestamp);


/**
 * @brief Sets the value of a Resource.
 *
 * If the Resource has no Datapoint, one will be created.  Then the
 * passed-in value and timestamp are copied to the Resource's first
 * Datapoint, and the Datapoint is marked dirty.
 *
 * @param[in] resource The Resource to set.
 * @param[in] content The new content.
 * @param[in] timestamp The Timestamp, if NULL the current time will be used
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
int bionet_resource_set_double(bionet_resource_t *resource, 
			       double content, 
			       const struct timeval *timestamp);


/**
 * @brief Sets the value of a Resource.
 *
 * If the Resource has no Datapoint, one will be created.  Then the
 * passed-in value and timestamp are copied to the Resource's first
 * Datapoint, and the Datapoint is marked dirty.
 *
 * @param[in] resource The Resource to set.
 * @param[in] content The new content.
 * @param[in] timestamp The Timestamp, if NULL the current time will be used
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
int bionet_resource_set_str(bionet_resource_t *resource, 
			    const char * content, 
			    const struct timeval *timestamp);


/**
 * @brief Add a datapoint to a resource
 *
 * @param[in] resource The resource to which the datapoint is added
 * @param[in] new_datapoint The datapoint to add
 */
void bionet_resource_add_datapoint(bionet_resource_t *resource, 
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


#endif // __BIONET_RESOURCE_H


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
