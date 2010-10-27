
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
// "COPYING.LESSER".  You should have received a copy of the GNU Lesser 
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

#ifndef BIONET_HAB_H
#define BIONET_HAB_H


#include "libbionet-util-decl.h"
#include <stdint.h>

/**
 * @file bionet-hab.h 
 * Functions for dealing with Bionet Hardware-Abstractor (HABs).
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Obtain a pointer to a new HAB
 *
 * @param[in] type Type of HAB or NULL. If NULL type is set to the name of 
 *                 the process
 * @param[in] id ID of HAB or NULL. If NULL ID is set to the hostname
 *
 * @return Valid pointer on success
 * @return NULL on failure
 */
BIONET_UTIL_API_DECL
bionet_hab_t *bionet_hab_new(const char *type, const char *id);


/**
 * @brief Get the Bionet qualified name of the HAB
 *
 * HAB name is of the format <HAB-Type>.<HAB-ID>
 *
 * @param[in] hab The HAB
 *
 * @return The HAB name.  This is a pointer to memory owned and managed by
 *         the Bionet library, the caller should not modify or free the
 *         returned pointer.
 *
 * @note This function is shorthand (and may be more efficient) for the following code. Internal 
 * library memory is used and cleaned up by the library when the HAB is free'd.
 * @code
 * char str[BIONET_NAME_COMPONENT_MAX_LEN * 2];
 * sprintf(str, "%s.%s", bionet_hab_get_type(hab), bionet_hab_get_id(hab));
 * @endcode
 */
BIONET_UTIL_API_DECL
const char *bionet_hab_get_name(const bionet_hab_t * hab);


/**
 * @brief Get the type of an existing HAB 
 *
 * @param[in] hab Pointer to a HAB
 * 
 * @return HAB-Type string
 * @return NULL on failure
 *
 * @note Do not free the returned pointer
 */
BIONET_UTIL_API_DECL
const char * bionet_hab_get_type(const bionet_hab_t *hab);


/**
 * @brief Get the ID of an existing HAB 
 *
 * @param[in] hab Pointer to a HAB
 * 
 * @return HAB-ID string
 * @return NULL on failure
 *
 * @note Do not free the returned pointer
 */
BIONET_UTIL_API_DECL
const char * bionet_hab_get_id(const bionet_hab_t *hab);


/**
 * @brief Get pointer to a node of a HAB by its ID
 *
 * @param[in] hab Pointer to a HAB
 * @param[in] node_id ID of the node requested 
 *
 * @retval Valid node pointer on success
 * @retval NULL if node with that ID does not exist
 */
BIONET_UTIL_API_DECL
bionet_node_t *bionet_hab_get_node_by_id(bionet_hab_t *hab, 
					 const char *node_id);

/**
 * @brief Get pointer to a node of a HAB by its ID, and UID
 *
 * @param[in] hab Pointer to a HAB
 * @param[in] node_id ID of the node requested 
 * @param[in] node_uid ID of the node requested 
 *
 * @retval Valid node pointer on success
 * @retval NULL if node with that ID and UID does not exist
 */
bionet_node_t *bionet_hab_get_node_by_id_and_uid(
        bionet_hab_t *hab,
        const char *node_id,
        const uint8_t node_uid[BDM_UUID_LEN]);


/**
 * @brief Get the number of nodes in the HAB
 * 
 * @param[in] hab Pointer to a HAB
 * 
 * @retval -1 Invalid HAB pointer
 * @retval Number of nodes in the HAB on success
 */
BIONET_UTIL_API_DECL
int bionet_hab_get_num_nodes(const bionet_hab_t *hab);


/**
 * @brief Get a node by its index in the HABs list
 * 
 * Useful for iterating over all the nodes in a HAB
 *
 * @param[in] hab Pointer to a HAB
 * @param[in] index Index of the node desired
 *
 * @retval NULL Invalid HAB pointer or index is greater than number of nodes
 * @retval Valid node pointer on success
 */
BIONET_UTIL_API_DECL
bionet_node_t *bionet_hab_get_node_by_index(const bionet_hab_t *hab, 
					    unsigned int index);


/**
 * @brief Add a node to the HAB
 *
 * @param[in] hab Pointer to a HAB
 * @param[in] node Pointer to a node
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
BIONET_UTIL_API_DECL
int bionet_hab_add_node(bionet_hab_t *hab, const bionet_node_t *node);


/**
 * @brief Remove a specific node from a HAB
 * 
 * @param[in] hab Pointer to a HAB
 * @param[in] node_id ID of node to remove
 *
 * @return 0 Success
 * @return -1 Failure
 *
 * @note Node is not free'd. The caller needs to free.
 */
BIONET_UTIL_API_DECL
bionet_node_t * bionet_hab_remove_node_by_id(bionet_hab_t *hab, const char *node_id);

/**
 * @brief Remove a specific node from a HAB
 * 
 * @param[in] hab Pointer to a HAB
 * @param[in] node_id ID of node to remove
 * @param[in] node_uid UID of node to remove
 *
 * @return 0 Success
 * @return -1 Failure
 *
 * @note Node is not free'd. The caller needs to free.
 */
BIONET_UTIL_API_DECL
bionet_node_t * bionet_hab_remove_node_by_id_and_uid(
        bionet_hab_t *hab,
        const char *node_id,
        const uint8_t node_uid[BDM_UUID_LEN]);

/**
 * @brief Remove all nodes from a HAB
 *
 * @param[in] hab Pointer to a HAB
 *
 * @return 0 Success
 * @return -1 Failure
 *
 * @note As a side-effect all nodes are free'd
 */
BIONET_UTIL_API_DECL
int bionet_hab_remove_all_nodes(bionet_hab_t *hab);


/**
 * @brief Free a HAB
 *
 * @param[in] hab Pointer to a HAB
 *
 * @note As a side-effect all associated nodes are removed and free'd
 */
BIONET_UTIL_API_DECL
void bionet_hab_free(bionet_hab_t *hab);


/**
 * @brief Checks if a HAB matches a HAB-Type and HAB-ID pair.
 *
 * @param[in] hab The HAB to test
 * @param[in] type The HAB-Type to compare
 * @param[in] id The HAB-ID to compare
 *
 * @return 0 Failed match
 * @return 1 Successful match
 *
 * @note The wildcard "*" matches any string.
 */
BIONET_UTIL_API_DECL
int bionet_hab_matches_type_and_id(const bionet_hab_t *hab, 
				   const char *type, 
				   const char *id);


/**
 * @brief Set the user-data annotation of a Hab
 *
 * @param[in] hab The Hab
 * @param[in] user_data The data to annotate the Hab with.
 *
 * @note If the user sets the user data, then the user is 
 *       responsible for freeing the it and setting it to 
 *       NULL before the hab is free'd.
 */
BIONET_UTIL_API_DECL
void bionet_hab_set_user_data(bionet_hab_t *hab, const void *user_data);


/**
 * @brief Get the user-data annotation of a Hab
 *
 * @param[in] hab The Hab
 *
 * @return The user_data pointer, or NULL if none has been set.
 */
BIONET_UTIL_API_DECL
void *bionet_hab_get_user_data(const bionet_hab_t *hab);


/**
 * @brief Check if communicating with the HAB over a secure connection
 * 
 * @param[in] hab The HAB
 * 
 * @retval -1 Invalid HAB pointer
 * @retval 0 Insecure
 * @retval 1 Secure
 *
 * @note This function does not make sense for HABs, only Bionet Clients.
 */
BIONET_UTIL_API_DECL
int bionet_hab_is_secure(const bionet_hab_t *hab);


/**
 * @brief Get the number of events in the HAB
 * 
 * @param[in] hab Pointer to a HAB
 * 
 * @retval -1 Invalid HAB pointer
 * @retval Number of events in the HAB on success
 */
BIONET_UTIL_API_DECL
int bionet_hab_get_num_events(const bionet_hab_t *hab);


/**
 * @brief Get a event by its index in the HABs list
 * 
 * Useful for iterating over all the events in a HAB
 *
 * @param[in] hab Pointer to a HAB
 * @param[in] index Index of the event desired
 *
 * @retval NULL Invalid HAB pointer or index is greater than number of events
 * @retval Valid event pointer on success
 */
BIONET_UTIL_API_DECL
bionet_event_t *bionet_hab_get_event_by_index(const bionet_hab_t *hab, 
					    unsigned int index);


/**
 * @brief Add a event to the HAB
 *
 * @param[in] hab Pointer to a HAB
 * @param[in] event Pointer to a event
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
BIONET_UTIL_API_DECL
int bionet_hab_add_event(bionet_hab_t *hab, const bionet_event_t *event);


/**
 * @brief Add a destruction notifier
 *
 * Each destruction notifier will be called in the order they
 * were added when bionet_hab_free() is called.
 *
 * @param[in] hab HAB to add the destructor for
 * @param[in] destructor The destructor to run.
 * @param[in] user_data User data to pass into the destructor.
 *
 * @retval 0 Successfully added.
 * @retval 1 Failed to add.
 */
BIONET_UTIL_API_DECL
int bionet_hab_add_destructor(bionet_hab_t * hab, 
			      void (*destructor)(bionet_hab_t * hab, void * user_data),
			      void * user_data);

/**
 * @brief Increment the reference count
 *
 * This function is used by wrappers of this interface. It is not
 * needed for writing C, but is for SWIG-generated Python so that
 * garbage collection works properly.
 *
 * @param[in] hab Hab to increment the reference count for
 */
BIONET_UTIL_API_DECL
void bionet_hab_increment_ref_count(bionet_hab_t * hab);


/**
 * @brief Get the reference count
 *
 * This function is used by wrappers of this interface. It is not
 * needed for writing C, but is for SWIG-generated Python so that
 * garbage collection works properly.
 *
 * @param[in] hab Hab to get the reference count for
 *
 * @return Number of references currently held.
 */
BIONET_UTIL_API_DECL
unsigned int bionet_hab_get_ref_count(bionet_hab_t * hab);


#ifdef __cplusplus
}
#endif

#endif //  BIONET_HAB_H


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
