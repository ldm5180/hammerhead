
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

#ifndef BIONET_NODE_H
#define BIONET_NODE_H

/**
 * @file bionet-node.h 
 * Functions for dealing with nodes
 */


#include "libbionet-util-decl.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Create a new node
 *
 * @param[in] hab The HAB which the node shall belong to
 * @param[in] node_id ID of the new node 
 *
 * @return Pointer to the new node
 * @retval NULL Failure
 */
BIONET_UTIL_API_DECL
bionet_node_t* bionet_node_new(bionet_hab_t *hab, const char* node_id);


/**
 * @brief Get the Bionet qualified name of the Node
 *
 * Node name is of the format <HAB-Type>.<HAB-ID>.<Node-ID>
 *
 * @param[in] node The Node
 *
 * @return The Node name, or NULL on error.
 *
 * @note This function is shorthand (and may be more efficient) for the following code. Internal 
 * library memory is used and cleaned up by the library when the node is free'd.
 * @code
 * char str[BIONET_NAME_COMPONENT_MAX_LEN * 3];
 * sprintf(str, "%s.%s.%s", 
 *         bionet_hab_get_type(bionet_node_get_hab(node)) 
 *         bionet_hab_get_id(bionet_node_get_hab(node))
 *         bionet_node_get_id(node));
 * @endcode
 */
BIONET_UTIL_API_DECL
const char *bionet_node_get_name(const bionet_node_t * node);


/**
 * @brief Get the ID of an existing Node 
 *
 * @param[in] node Pointer to a Node
 * 
 * @return Node-ID string
 * @return NULL on failure
 *
 * @note Do not free the returned pointer
 */
BIONET_UTIL_API_DECL
const char * bionet_node_get_id(const bionet_node_t *node);

/**
 * @brief Get the UUID of an existing Node 
 *
 * @param[in] node Pointer to a Node
 * 
 * @return UUID as a pointer to an BDM_UUID_LEN byte char array
 * @return NULL on failure
 *
 * @note Do not free the returned pointer
 */
BIONET_UTIL_API_DECL
const uint8_t * bionet_node_get_uid(const bionet_node_t *node);


/**
 * @brief Get the parent HAB of a node
 *
 * @param[in] node Pointer to a Node
 * 
 * @return Pointer to a HAB
 * @return NULL on failure
 */
BIONET_UTIL_API_DECL
bionet_hab_t * bionet_node_get_hab(const bionet_node_t *node);


/**
 * @brief Add a resource to a node
 *
 * @param[in] node The node to add the resource to
 * @param[in] resource The resource to add
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
BIONET_UTIL_API_DECL
int bionet_node_add_resource(bionet_node_t *node, bionet_resource_t *resource);


/**
 * @brief Get the number of resource on a node
 *
 * @param[in] node Node to query
 *
 * @retval -1 Failure
 * @retval >=0 Number of nodes
 */
BIONET_UTIL_API_DECL
int bionet_node_get_num_resources(const bionet_node_t *node);


/**
 * @brief Get a resource by its index in the node's list
 *
 * This can be used to iterate through all the resources for a node
 *
 * @param[in] node Node to query
 * @param[in] index Index of the resource desired
 *
 * @return Pointer to a resource
 * @retval NULL Failure
 */
BIONET_UTIL_API_DECL
bionet_resource_t *bionet_node_get_resource_by_index(const bionet_node_t *node,
						     unsigned int index);


/**
 * @brief Get a resource by ID
 *
 * @param[in] node Node to query
 * @param[in] resource_id ID of resource desired
 *
 * @return Pointer to a resource
 * @retval NULL resource does not exist or Failure
 */
BIONET_UTIL_API_DECL
bionet_resource_t *bionet_node_get_resource_by_id(const bionet_node_t *node,
						  const char *resource_id);


/**
 * @brief Add a Stream to a node
 *
 * @param[in] node The Node to add the Stream to
 * @param[in] stream The Stream to add
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
BIONET_UTIL_API_DECL
int bionet_node_add_stream(bionet_node_t *node, bionet_stream_t *stream);


/**
 * @brief Get the number of streams for a node
 * 
 * @param[in] node Node to query
 *
 * @retval -1 Failure
 * @retval >=0 Number of streams
 */
BIONET_UTIL_API_DECL
int bionet_node_get_num_streams(const bionet_node_t *node);


/**
 * @brief Get a stream by its index in the node's list
 *
 * This can be used to iterate through all the streams for a node
 *
 * @param[in] node Node to query
 * @param[in] index Index of the stream desired
 *
 * @return Pointer to a stream
 * @retval NULL Failure
 */
BIONET_UTIL_API_DECL
bionet_stream_t *bionet_node_get_stream_by_index(const bionet_node_t *node, unsigned int index);


/**
 * @brief Get a stream by ID
 *
 * @param[in] node Node to query
 * @param[in] stream_id ID of stream desired
 *
 * @return Pointer to a stream
 * @retval NULL stream does not exist or Failure
 */
BIONET_UTIL_API_DECL
bionet_stream_t *bionet_node_get_stream_by_id(const bionet_node_t *node, const char *stream_id);


/**
 * @brief Free a node
 *
 * As a side-effect all resources and streams will also be free'd
 *
 * @param[in] node Node to free
 *
 * @note This function will not remove the node from any HABs. If it 
 * has been added to a HAB it should be removed using 
 * bionet_hab_remove_node_by_id() before calling this function.
 */
BIONET_UTIL_API_DECL
void bionet_node_free(bionet_node_t *node);


/**
 * @brief Checks if a Node matches a name specification.
 *
 * @see bionet_node_matches_habtype_habid_nodeid()
 *
 * @param[in] node The Node to check.
 * @param[in] id The Node-ID to check against.  
 *
 * @retval FALSE (zero) - The Node does not match the ID 
 * @retval TRUE (non-zero) - The node matches the ID
 *
 * @note Any string may be replaced by the wildcard "*" to match all
 */
BIONET_UTIL_API_DECL
int bionet_node_matches_id(const bionet_node_t *node, const char *id);


/**
 * @brief Checks if a Node matches a name specification.
 *
 * @see bionet_node_matches_id()
 *
 * @param[in] node  The Node to check.
 * @param[in] hab_type  The HAB-Type to check against.  
 * @param[in] hab_id The HAB-ID to check against. 
 * @param[in] node_id The Node-ID to check against.
 *
 * @retval FALSE (zero) - The Node does not match the ID 
 * @retval TRUE (non-zero) - The node matches the ID
 *
 * @note Any string may be replaced by the wildcard "*" to match all
 */
BIONET_UTIL_API_DECL
int bionet_node_matches_habtype_habid_nodeid(const bionet_node_t *node, 
					     const char *hab_type, 
					     const char *hab_id, 
					     const char *node_id);


/**
 * @brief Set the user-data annotation of a Node
 *
 * @param[in] node The Node
 * @param[in] user_data The data to annotate the Node with.
 *
 * @note If the user sets the user data, then the user is 
 *       responsible for freeing the it and setting it to 
 *       NULL before the hab is free'd.
 */
BIONET_UTIL_API_DECL
void bionet_node_set_user_data(bionet_node_t *node, const void *user_data);


/**
 * @brief Get the user-data annotation of a Node
 *
 * @param[in] node The Node
 *
 * @return The user_data pointer, or NULL if none has been set.
 */
BIONET_UTIL_API_DECL
void *bionet_node_get_user_data(const bionet_node_t *node);


/**
 * @brief Get the number of events in the Node
 * 
 * @param[in] node Pointer to a Node
 * 
 * @retval -1 Invalid Node pointer
 * @retval Number of events in the Node on success
 */
BIONET_UTIL_API_DECL
int bionet_node_get_num_events(const bionet_node_t *node);


/**
 * @brief Get a event by its index in the Nodes list
 * 
 * Useful for iterating over all the events in a Node
 *
 * @param[in] node Pointer to a Node
 * @param[in] index Index of the event desired
 *
 * @retval NULL Invalid Node pointer or index is greater than number of events
 * @retval Valid event pointer on success
 */
BIONET_UTIL_API_DECL
bionet_event_t *bionet_node_get_event_by_index(const bionet_node_t *node, 
					    unsigned int index);


/**
 * @brief Add a event to the Node
 *
 * @param[in] node Pointer to a Node
 * @param[in] event Pointer to a event
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
BIONET_UTIL_API_DECL
int bionet_node_add_event(bionet_node_t *node, const bionet_event_t *event);


/**
 * @brief Add a destruction notifier
 *
 * Each destruction notifier will be called in the order they
 * were added when bionet_node_free() is called.
 *
 * @param[in] node Resource to add the destructor for
 * @param[in] destructor The destructor to run.
 * @param[in] user_data User data to pass into the destructor.
 *
 * @retval 0 Successfully added.
 * @retval 1 Failed to add.
 */
BIONET_UTIL_API_DECL
int bionet_node_add_destructor(bionet_node_t * resource, 
			       void (*destructor)(bionet_node_t * node, void * user_data),
			       void * user_data);


#ifdef __cplusplus
}
#endif

#endif //  BIONET_NODE_H

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
