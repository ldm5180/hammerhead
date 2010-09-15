
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

// This library is free software. You can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as 
// published by the Free Software Foundation, version 2.1 of the License.
// This library is distributed in the hope that it will be useful, but 
// WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details. A copy of the GNU 
// Lesser General Public License v 2.1 can be found in the file named 
// “COPYING.LESSER”.  You should have received a copy of the GNU Lesser 
// General Public License along with this library; if not, write to the 
// Free Software Foundation, Inc., 
// 51 Franklin Street, Fifth Floor, 
// Boston, MA 02110-1301 USA.
 
// You may contact the Automation Group at:
// bionet@bioserve.colorado.edu
 
// Dr. Kevin Gifford
// University of Colorado
// Engineering Center, ECAE 1B08
// Boulder, CO 80309
 
// Because BioNet was developed at a university, we ask that you provide
// attribution to the BioNet authors in any redistribution, modification, 
// work, or article based on this library.
 
// You may contribute modifications or suggestions to the University of
// Colorado for the purpose of discussing and improving this software.
// Before your modifications are incorporated into the master version 
// distributed by the University of Colorado, we must have a contributor
// license agreement on file from each contributor. If you wish to supply
// the University with your modifications, please join our mailing list.
// Instructions can be found on our website at 
// http://bioserve.colorado.edu/developers-corner.

#ifndef __BIONET_DATAPOINT_H
#define __BIONET_DATAPOINT_H

/**
 * @file bionet-datapoint.h
 * Functions for dealing with Bionet Datapoints.
 */


#include "libbionet-util-decl.h"


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
BIONET_UTIL_API_DECL
bionet_datapoint_t *bionet_datapoint_new(bionet_resource_t *resource,
					 bionet_value_t *value,
					 const struct timeval *timestamp);


/**
 * @brief Frees a Datapoint created with bionet_datapoint_new()
 *
 * @param[in] datapoint The Datapoint to free.
 */
BIONET_UTIL_API_DECL
void bionet_datapoint_free(bionet_datapoint_t *datapoint);


/**
 * @brief Set the value of a datapoint
 *
 * The datapoint's existing bionet_value_t (if any) is freed, and replaced
 * with the passed-in bionet_value_t, which becomes the property of the
 * datapoint.
 *
 * @param[in] datapoint Datapoint to set
 * @param[in] value Value to set in the datapoint
 */
BIONET_UTIL_API_DECL
void bionet_datapoint_set_value(bionet_datapoint_t *datapoint, bionet_value_t *value);


/**
 * @brief Get the value of a datapoint
 *
 * @param[in] datapoint Datapoint to set
 *
 * @retval Pointer to Value
 * @retval NULL on failure
 */
BIONET_UTIL_API_DECL
bionet_value_t * bionet_datapoint_get_value(const bionet_datapoint_t *datapoint);


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
 * @code
 * bionet_value_get_resource(bionet_datapoint_get_value(datapoint))
 * @endcode
 */
BIONET_UTIL_API_DECL
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
 * @code
 * bionet_resource_get_node(bionet_value_get_resource(bionet_datapoint_get_value(datapoint)))
 * @endcode
 */
BIONET_UTIL_API_DECL
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
 * @code
 * bionet_node_get_hab(bionet_resource_get_node(bionet_value_get_resource(bionet_datapoint_get_value(datapoint))))
 * @endcode
 */
BIONET_UTIL_API_DECL
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
BIONET_UTIL_API_DECL
const char *bionet_datapoint_timestamp_to_string(const bionet_datapoint_t *datapoint);


/**
 * @brief Sets the Datapoint Timestamp as specified.
 *
 * The datapoint's timestamp is overwritten with the contents of the new
 * timestamp.  The new_timestamp argument is unmodified and, unlike the new
 * value of bionet_datapoint_set_value(), remains the property of the
 * caller.
 *
 * @param[in] datapoint The Datapoint to set the Timestamp of
 * @param[in] new_timestamp Timestamp to set it to (NULL means "now").
 */
BIONET_UTIL_API_DECL
void bionet_datapoint_set_timestamp(bionet_datapoint_t *datapoint, 
				    const struct timeval *new_timestamp);


/**
 * @brief Gets the Datapoint Timestamp.
 *
 * @param[in] datapoint The Datapoint to get the Timestamp of.
 *
 * @return Pointer to timestamp (do not modify or free).
 * @return NULL on failure
 */
BIONET_UTIL_API_DECL
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
BIONET_UTIL_API_DECL
int bionet_datapoint_is_dirty(const bionet_datapoint_t *datapoint);


/**
 * @brief Makes a Datapoint clean.
 *
 * @param datapoint The Datapoint to make clean.
 */
BIONET_UTIL_API_DECL
void bionet_datapoint_make_clean(bionet_datapoint_t *datapoint);

/**
 * @brief Check id datapoints are identical
 *
 * @return 0 if datapoint timestamp and value are identical
 * @return <>0 otherwise
 */
BIONET_UTIL_API_DECL
int bionet_datapoint_iseq(const bionet_datapoint_t *dp1, const bionet_datapoint_t *dp2);


/**
 * @brief Compare timevals like strcmp
 *
 * @return <0 if tva < tvb
 * @return 0 if tva == tvb
 * @return >0 if tva > tvb
 */
BIONET_UTIL_API_DECL
int bionet_timeval_compare(const struct timeval * tva, const struct timeval * tvb);


/**
 * @brief Subtract timevals
 *
 * @param[in] tva Minuend
 * @param[in] tvb Subtrahend
 *
 * @return Difference
 *
 * @note return = tva - tvb
 */
BIONET_UTIL_API_DECL
struct timeval bionet_timeval_subtract(const struct timeval * tva, const struct timeval *tvb);


/**
 *
 */
BIONET_UTIL_API_DECL
bionet_datapoint_t * bionet_datapoint_dup(bionet_datapoint_t * datapoint);


/**
 * @brief Get the number of events in the Datapoint
 * 
 * @param[in] datapoint Pointer to a Datapoint
 * 
 * @retval -1 Invalid Datapoint pointer
 * @retval Number of events in the Datapoint on success
 */
BIONET_UTIL_API_DECL
int bionet_datapoint_get_num_events(const bionet_datapoint_t *datapoint);


/**
 * @brief Get a event by its index in the Datapoints list
 * 
 * Useful for iterating over all the events in a Datapoint
 *
 * @param[in] datapoint Pointer to a Datapoint
 * @param[in] index Index of the event desired
 *
 * @retval NULL Invalid Datapoint pointer or index is greater than number of events
 * @retval Valid event pointer on success
 */
BIONET_UTIL_API_DECL
bionet_event_t *bionet_datapoint_get_event_by_index(const bionet_datapoint_t *datapoint, 
					    unsigned int index);


/**
 * @brief Add a event to the Datapoint
 *
 * @param[in] datapoint Pointer to a Datapoint
 * @param[in] event Pointer to a event
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
BIONET_UTIL_API_DECL
int bionet_datapoint_add_event(bionet_datapoint_t *datapoint, const bionet_event_t *event);


/**
 * @brief Add a destruction notifier
 *
 * Each destruction notifier will be called in the order they
 * were added when bionet_datapoint_free() is called.
 *
 * @param[in] datapoint Resource to add the destructor for
 * @param[in] destructor The destructor to run.
 * @param[in] user_data User data to pass into the destructor.
 *
 * @retval 0 Successfully added.
 * @retval 1 Failed to add.
 */
BIONET_UTIL_API_DECL
int bionet_datapoint_add_destructor(bionet_datapoint_t * datapoint, 
				    void (*destructor)(bionet_datapoint_t * datapoint, void * user_data),
				    void * user_data);


#endif /* __BIONET_DATAPOINT_H */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
