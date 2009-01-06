
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef __BIONET_DATAPOINT_H
#define __BIONET_DATAPOINT_H

/**
 * @file bionet-datapoint.h
 * Functions for dealing with Bionet Datapoints.
 */


#include <stdint.h>
#include <sys/time.h>

#include <glib.h>

#include "bionet-util.h"


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
bionet_datapoint_t *bionet_datapoint_new(bionet_resource_t *resource,
					 bionet_value_t *value,
					 const struct timeval *timestamp);


/**
 * @brief Frees a Datapoint created with bionet_datapoint_new()
 *
 * @param[in] datapoint The Datapoint to free.
 */
void bionet_datapoint_free(bionet_datapoint_t *datapoint);


/**
 * @brief Set the value of a datapoint
 *
 * @param[in] d Datapoint to set
 * @param[in] value Value to set in the datapoint
 */
void bionet_datapoint_set_value(bionet_datapoint_t *datapoint, bionet_value_t *value);


/**
 * @brief Get the value of a datapoint
 *
 * @param[in] d Datapoint to set
 *
 * @retval Pointer to Value
 * @retval NULL on failure
 */
bionet_value_t * bionet_datapoint_get_value(bionet_datapoint_t *datapoint);


/**
 * @brief Get the resource of a datapoint
 *
 * @param[in] datapoint The datapoint
 *
 * @return Pointer to Resource
 * @return NULL Error or is not part of a resource
 *
 * @note This is shorthand for getting the value, then getting the
 * resource of the value
 */
bionet_resource_t * bionet_datapoint_get_resource(const bionet_datapoint_t * datapoint);


/**
 * @brief Get the node of a datapoint
 *
 * @param[in] datapoint The datapoint
 *
 * @return Pointer to Node
 * @return NULL Error or is not part of a node
 *
 * @note This is shorthand for getting the value, then getting the
 * resource of the value, then getting the node of the resource
 */
bionet_node_t * bionet_datapoint_get_node(const bionet_datapoint_t * datapoint);


/**
 * @brief Get the HAB of a datapoint
 *
 * @param[in] datapoint The datapoint
 *
 * @return Pointer to HAB
 * @return NULL Error or is not part of a HAB
 *
 * @note This is shorthand for getting the value, then getting the
 * resource of the value, then getting the node of the resource, then
 * getting the HAB of the node
 */
bionet_hab_t * bionet_datapoint_get_hab(const bionet_datapoint_t * datapoint);


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
 * @brief Sets the Datapoint Timestamp as specified.
 *
 * @param[in] datapoint The Datapoint to set the Timestamp of
 *
 * @return Pointer to timestamp
 * @return NULL on failure
 */
struct timeval * bionet_datapoint_get_timestamp(bionet_datapoint_t *datapoint);


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


#endif /* __BIONET_DATAPOINT_H */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
