
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef __BIONET_RESOURCE_H
#define __BIONET_RESOURCE_H

/**
 * @file bionet-resource.h
 * Functions for dealing with Bionet Resources.
 */


#include "bionet-platform.h"

#include "libbionet-util-decl.h"

#ifdef __cplusplus
extern "C" {
#endif

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
BIONET_UTIL_API_DECL
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
BIONET_UTIL_API_DECL
void bionet_resource_free(bionet_resource_t *resource);


/**
 * @brief Get the Bionet qualified name of the Resource
 *
 * Resource name is of the format <HAB-Type>.<HAB-ID>.<Node-ID>:<Resource-ID>
 *
 * @param[in] resource The Resource
 *
 * @return The Resource Name, or NULL on error
 *
 * @note This function is shorthand (and may be more efficient) for the following code. Internal 
 * library memory is used and cleaned up by the library when the node is free'd.
 * @code
 * char str[BIONET_NAME_COMPONENT_MAX_LEN * 4];
 * sprintf(str, "%s.%s.%s:%s", 
 *         bionet_hab_get_type(bionet_resource_get_hab(resource)), 
 *         bionet_hab_get_id(bionet_resource_get_hab(resource)),
 *         bionet_node_get_id(bionet_resource_get_node(resource)),
 *         bionet_resource_get_id(resource));
 * @endcode
 */
BIONET_UTIL_API_DECL
const char *bionet_resource_get_name(const bionet_resource_t * resource);


/**
 * @brief Get the local-to-the-HAB name of a Resource
 *
 * The local Resource name is of the format <Node-ID>:<Resource-ID>
 *
 * @param[in] resource The Resource
 *
 * @return The local Resource Name, or NULL on error
 *
 * @note This function is shorthand (and may be more efficient) for the following code. Internal 
 * library memory is used and cleaned up by the library when the node is free'd.
 * @code
 * char str[BIONET_NAME_COMPONENT_MAX_LEN * 2];
 * sprintf(str, "%s:%s", 
 *         bionet_node_get_id(bionet_resource_get_node(resource)),
 *         bionet_resource_get_id(resource));
 * @endcode
 */
BIONET_UTIL_API_DECL
const char *bionet_resource_get_local_name(const bionet_resource_t * resource);


/**
 * @brief Get ID of a resource
 *
 * @param[in] resource The resource
 *
 * @return ID of the resource
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
const char *bionet_resource_get_id(bionet_resource_t *resource);


/**
 * @brief Get the node this resource belongs to
 *
 * @param[in] resource The Resource
 *
 * @return Pointer to Node
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_node_t * bionet_resource_get_node(const bionet_resource_t *resource);


/**
 * @brief Get the HAB this resource belongs to
 *
 * @param[in] resource The Resource
 *
 * @return Pointer to HAB
 * @return NULL Error
 *
 * @note This is shorthand (and may be more efficient) for obtaining the Node 
 * and then obtaining Node's HAB:
 * @code
 * bionet_node_get_hab(bionet_resource_get_node(resource));
 * @endcode
 */
BIONET_UTIL_API_DECL
bionet_hab_t * bionet_resource_get_hab(const bionet_resource_t *resource);


/**
 * @brief Get the data type of a resource
 *
 * @param[in] resource The resource
 *
 * @return Type of resource
 */
BIONET_UTIL_API_DECL
bionet_resource_data_type_t bionet_resource_get_data_type(const bionet_resource_t *resource);


/**
 * @brief Get the flavor of a resource
 *
 * @param[in] resource
 *
 * @return Flavor of resource
 */
BIONET_UTIL_API_DECL
bionet_resource_flavor_t bionet_resource_get_flavor(const bionet_resource_t *resource);


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
BIONET_UTIL_API_DECL
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
BIONET_UTIL_API_DECL
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
BIONET_UTIL_API_DECL
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
BIONET_UTIL_API_DECL
bionet_resource_data_type_t bionet_resource_data_type_from_string(const char *data_type_string);


/**
 * @brief Sets the value of a Resource (of any data type).
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
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_resource_remove_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_value_dup(resource, content);
 * bionet_datapoint_t *dp = bionet_datapoint_new(resource, value, ts);
 * bionet_resource_add_datapoint(resource, dp);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_set(bionet_resource_t *resource, 
			const bionet_value_t *content, 
			const struct timeval *timestamp);


/**
 * @brief Sets the value of a Binary Resource.
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
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_resource_remove_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_value_new_binary(resource, content);
 * bionet_datapoint_t *dp = bionet_datapoint_new(resource, value, ts);
 * bionet_resource_add_datapoint(resource, dp);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_set_binary(bionet_resource_t *resource, 
			       int content, 
			       const struct timeval *timestamp);


/**
 * @brief Sets the value of a UInt8 Resource.
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
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_resource_remove_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_value_new_uint8(resource, content);
 * bionet_datapoint_t *dp = bionet_datapoint_new(resource, value, ts);
 * bionet_resource_add_datapoint(resource, dp);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_set_uint8(bionet_resource_t *resource, 
			      uint8_t content, 
			      const struct timeval *timestamp);


/**
 * @brief Sets the value of an Int8 Resource.
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
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_resource_remove_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_value_new_int8(resource, content);
 * bionet_datapoint_t *dp = bionet_datapoint_new(resource, value, ts);
 * bionet_resource_add_datapoint(resource, dp);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_set_int8(bionet_resource_t *resource, 
			     int8_t content, 
			     const struct timeval *timestamp);


/**
 * @brief Sets the value of a UInt16 Resource.
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
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_resource_remove_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_value_new_uint16(resource, content);
 * bionet_datapoint_t *dp = bionet_datapoint_new(resource, value, ts);
 * bionet_resource_add_datapoint(resource, dp);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_set_uint16(bionet_resource_t *resource, 
			       uint16_t content, 
			       const struct timeval *timestamp);


/**
 * @brief Sets the value of an Int16 Resource.
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
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_resource_remove_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_value_new_int16(resource, content);
 * bionet_datapoint_t *dp = bionet_datapoint_new(resource, value, ts);
 * bionet_resource_add_datapoint(resource, dp);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_set_int16(bionet_resource_t *resource, 
			      int16_t content, 
			      const struct timeval *timestamp);


/**
 * @brief Sets the value of a UInt32 Resource.
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
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_resource_remove_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_value_new_uint32(resource, content);
 * bionet_datapoint_t *dp = bionet_datapoint_new(resource, value, ts);
 * bionet_resource_add_datapoint(resource, dp);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_set_uint32(bionet_resource_t *resource, 
			       uint32_t content, 
			       const struct timeval *timestamp);


/**
 * @brief Sets the value of an Int32 Resource.
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
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_resource_remove_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_value_new_int32(resource, content);
 * bionet_datapoint_t *dp = bionet_datapoint_new(resource, value, ts);
 * bionet_resource_add_datapoint(resource, dp);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_set_int32(bionet_resource_t *resource, 
			      int32_t content, 
			      const struct timeval *timestamp);


/**
 * @brief Sets the value of a Float Resource.
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
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_resource_remove_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_value_new_float(resource, content);
 * bionet_datapoint_t *dp = bionet_datapoint_new(resource, value, ts);
 * bionet_resource_add_datapoint(resource, dp);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_set_float(bionet_resource_t *resource, 
			      float content, 
			      const struct timeval *timestamp);


/**
 * @brief Sets the value of a Double Resource.
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
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_resource_remove_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_value_new_double(resource, content);
 * bionet_datapoint_t *dp = bionet_datapoint_new(resource, value, ts);
 * bionet_resource_add_datapoint(resource, dp);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_set_double(bionet_resource_t *resource, 
			       double content, 
			       const struct timeval *timestamp);


/**
 * @brief Sets the value of a String Resource.
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
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_resource_remove_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_value_new_str(resource, content);
 * bionet_datapoint_t *dp = bionet_datapoint_new(resource, value, ts);
 * bionet_resource_add_datapoint(resource, dp);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_set_str(bionet_resource_t *resource, 
			    const char * content, 
			    const struct timeval *timestamp);


/**
 * @brief Gets the value of a Resource.
 *
 * Gets the value of the Resource's first datapoint (if any).
 *
 * @param[in] resource The Resource to get.
 * @param[in] content The value will be written here.
 * @param[in] timestamp The timestamp will be written here.
 *
 * @retval 0 Success
 * @retval -1 Failure
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_datapoint_t *dp = bionet_resource_get_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_datapoint_get_value(datapoint);
 * timestamp = bionet_datapoint_get_timestamp(datapoint);
 * bionet_value_get_binary(value, &content);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_get_binary(bionet_resource_t *resource, 
			       int *content, 
			       struct timeval *timestamp);


/**
 * @brief Gets the value of a Resource.
 *
 * Gets the value of the Resource's first datapoint (if any).
 *
 * @param[in] resource The Resource to get.
 * @param[in] content The value will be written here.
 * @param[in] timestamp The timestamp will be written here.
 *
 * @retval 0 Success
 * @retval -1 Failure
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_datapoint_t *dp = bionet_resource_get_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_datapoint_get_value(datapoint);
 * timestamp = bionet_datapoint_get_timestamp(datapoint);
 * bionet_value_get_uint8(value, &content);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_get_uint8(bionet_resource_t *resource, 
			      uint8_t *content, 
			      struct timeval *timestamp);


/**
 * @brief Gets the value of a Resource.
 *
 * Gets the value of the Resource's first datapoint (if any).
 *
 * @param[in] resource The Resource to get.
 * @param[in] content The value will be written here.
 * @param[in] timestamp The timestamp will be written here.
 *
 * @retval 0 Success
 * @retval -1 Failure
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_datapoint_t *dp = bionet_resource_get_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_datapoint_get_value(datapoint);
 * timestamp = bionet_datapoint_get_timestamp(datapoint);
 * bionet_value_get_int8(value, &content);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_get_int8(bionet_resource_t *resource, 
			     int8_t *content, 
			     struct timeval *timestamp);


/**
 * @brief Gets the value of a Resource.
 *
 * Gets the value of the Resource's first datapoint (if any).
 *
 * @param[in] resource The Resource to get.
 * @param[in] content The value will be written here.
 * @param[in] timestamp The timestamp will be written here.
 *
 * @retval 0 Success
 * @retval -1 Failure
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_datapoint_t *dp = bionet_resource_get_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_datapoint_get_value(datapoint);
 * timestamp = bionet_datapoint_get_timestamp(datapoint);
 * bionet_value_get_uint16(value, &content);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_get_uint16(bionet_resource_t *resource, 
			       uint16_t *content, 
			       struct timeval *timestamp);


/**
 * @brief Gets the value of a Resource.
 *
 * Gets the value of the Resource's first datapoint (if any).
 *
 * @param[in] resource The Resource to get.
 * @param[in] content The value will be written here.
 * @param[in] timestamp The timestamp will be written here.
 *
 * @retval 0 Success
 * @retval -1 Failure
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_datapoint_t *dp = bionet_resource_get_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_datapoint_get_value(datapoint);
 * timestamp = bionet_datapoint_get_timestamp(datapoint);
 * bionet_value_get_int16(value, &content);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_get_int16(bionet_resource_t *resource, 
			      int16_t *content, 
			      struct timeval *timestamp);


/**
 * @brief Gets the value of a Resource.
 *
 * Gets the value of the Resource's first datapoint (if any).
 *
 * @param[in] resource The Resource to get.
 * @param[in] content The value will be written here.
 * @param[in] timestamp The timestamp will be written here.
 *
 * @retval 0 Success
 * @retval -1 Failure
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_datapoint_t *dp = bionet_resource_get_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_datapoint_get_value(datapoint);
 * timestamp = bionet_datapoint_get_timestamp(datapoint);
 * bionet_value_get_uint32(value, &content);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_get_uint32(bionet_resource_t *resource, 
			       uint32_t *content, 
			       struct timeval *timestamp);


/**
 * @brief Gets the value of a Resource.
 *
 * Gets the value of the Resource's first datapoint (if any).
 *
 * @param[in] resource The Resource to get.
 * @param[in] content The value will be written here.
 * @param[in] timestamp The timestamp will be written here.
 *
 * @retval 0 Success
 * @retval -1 Failure
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_datapoint_t *dp = bionet_resource_get_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_datapoint_get_value(datapoint);
 * timestamp = bionet_datapoint_get_timestamp(datapoint);
 * bionet_value_get_int32(value, &content);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_get_int32(bionet_resource_t *resource, 
			      int32_t *content, 
			      struct timeval *timestamp);


/**
 * @brief Gets the value of a Resource.
 *
 * Gets the value of the Resource's first datapoint (if any).
 *
 * @param[in] resource The Resource to get.
 * @param[in] content The value will be written here.
 * @param[in] timestamp The timestamp will be written here.
 *
 * @retval 0 Success
 * @retval -1 Failure
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_datapoint_t *dp = bionet_resource_get_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_datapoint_get_value(datapoint);
 * timestamp = bionet_datapoint_get_timestamp(datapoint);
 * bionet_value_get_float(value, &content);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_get_float(bionet_resource_t *resource, 
			      float *content, 
			      struct timeval *timestamp);


/**
 * @brief Gets the value of a Resource.
 *
 * Gets the value of the Resource's first datapoint (if any).
 *
 * @param[in] resource The Resource to get.
 * @param[in] content The value will be written here.
 * @param[in] timestamp The timestamp will be written here.
 *
 * @retval 0 Success
 * @retval -1 Failure
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_datapoint_t *dp = bionet_resource_get_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_datapoint_get_value(datapoint);
 * timestamp = bionet_datapoint_get_timestamp(datapoint);
 * bionet_value_get_double(value, &content);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_get_double(bionet_resource_t *resource, 
			       double *content, 
			       struct timeval *timestamp);


/**
 * @brief Gets the value of a Resource.
 *
 * Gets the value of the Resource's first datapoint (if any).
 *
 * @param[in] resource The Resource to get.
 * @param[in] content The value will be written here.  *content will be set
 *            to point to dynamically allocated memory, which becomes the
 *            responsibility of the caller.
 * @param[in] timestamp The timestamp will be written here.
 *
 * @retval 0 Success
 * @retval -1 Failure
 *
 * @note This is shorthand for the following code.
 * @code
 * bionet_datapoint_t *dp = bionet_resource_get_datapoint_by_index(resource, 0);
 * bionet_value_t *value = bionet_datapoint_get_value(datapoint);
 * timestamp = bionet_datapoint_get_timestamp(datapoint);
 * bionet_value_get_str(value, &content);
 * @endcode
 */
BIONET_UTIL_API_DECL
int bionet_resource_get_str(bionet_resource_t *resource, 
			    char * *content, 
			    struct timeval *timestamp);


/**
 * @brief Add a datapoint to a resource
 *
 * @param[in] resource The resource to which the datapoint is added
 * @param[in] new_datapoint The datapoint to add
 */
BIONET_UTIL_API_DECL
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
BIONET_UTIL_API_DECL
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
 *
 * @note Resources owned by Clients and HABs should only ever have 1 datapoint. When a resource is 
 * set to a new value the old datapoint is replaced by the Bionet library. @see 
 * BIONET_RESOURCE_GET_DATAPOINT(resource). Resources from the Bionet Data Manager may have more 
 * than 1 datapoint.
 */
BIONET_UTIL_API_DECL
bionet_datapoint_t *bionet_resource_get_datapoint_by_index(const bionet_resource_t *resource, unsigned int index);


/**
 * @def BIONET_RESOURCE_GET_DATAPOINT(resource)
 * Obtain the first (and usually only) datapoint from a resource. Shorthand for getting the 0th 
 * index datapoint.
 *
 * @see bionet_resource_get_datapoint_by_index()
 */
#define BIONET_RESOURCE_GET_DATAPOINT(resource) bionet_resource_get_datapoint_by_index(resource, 0)


/**
 * @brief Remove (and deallocate) a datapoint from a resource
 *
 * @param[in] resource The resource
 * @param[in] index Index of the datapoint to remove
 *
 */
BIONET_UTIL_API_DECL
void bionet_resource_remove_datapoint_by_index(bionet_resource_t *resource, unsigned int index);


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
BIONET_UTIL_API_DECL
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
BIONET_UTIL_API_DECL
int bionet_resource_matches_habtype_habid_nodeid_resourceid(
    const bionet_resource_t *resource,
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id
);


/**
 * @brief Set the user-data annotation of a Resource
 *
 * @param[in] resource The Resource
 * @param[in] user_data The data to annotate the Resource with.
 *
 * @note If the user sets the user data, then the user is 
 *       responsible for freeing the it and setting it to 
 *       NULL before the hab is free'd.
 */
BIONET_UTIL_API_DECL
void bionet_resource_set_user_data(bionet_resource_t *resource, const void *user_data);


/**
 * @brief Get the user-data annotation of a Resource
 *
 * @param[in] resource The Resource
 *
 * @return The user_data pointer, or NULL if none has been set.
 */
BIONET_UTIL_API_DECL
void *bionet_resource_get_user_data(const bionet_resource_t *resource);


/**
 * @brief Get the epsilon of a Resource
 * 
 * @param[in] resource The Resource
 *
 * @return The pointer to the epsilon, or NULL if none has been set.
 */
BIONET_UTIL_API_DECL
bionet_epsilon_t * bionet_resource_get_epsilon(const bionet_resource_t * resource);


/**
 * @brief Get the epsilon of a Resource
 * 
 * @param[in] resource The Resource
 *
 * @return The epsilon timeval.
 */
BIONET_UTIL_API_DECL
const struct timeval * bionet_resource_get_delta(const bionet_resource_t * resource);


/**
 * @brief Set the epsilon thresholds before resource gets published.
 *
 * By default resources get published any time they get set. This modifies
 * that behavior so that it only gets published once the change in the value
 * is greater than or equal to (>=) the epsilon or the time since the 
 * last published datapoint timestamp is greater than or equal to (>=)
 * the epsilon.
 *
 * @param[in] resource Bionet resource to set
 * @param[in] epsilon Difference in value
 *
 * @retval 0 Success.
 * @retval 1 Failure.
 */
BIONET_UTIL_API_DECL
int bionet_resource_set_epsilon(bionet_resource_t * resource,
				bionet_epsilon_t * epsilon);


/**
 * @brief Set the delta thresholds before resource gets published.
 *
 * By default resources get published any time they get set. This modifies
 * that behavior so that it only gets published once the change in the value
 * is greater than or equal to (>=) the epsilon or the time since the 
 * last published datapoint timestamp is greater than or equal to (>=)
 * the epsilon.
 *
 * @param[in] resource Bionet resource to set
 * @param[in] epsilon Difference in datapoint timestamp and current time
 *
 * @retval 0 Success.
 * @retval 1 Failure.
 */
BIONET_UTIL_API_DECL
int bionet_resource_set_delta(bionet_resource_t * resource,
			      struct timeval delta);

#ifdef __cplusplus
}
#endif

#endif // __BIONET_RESOURCE_H


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
