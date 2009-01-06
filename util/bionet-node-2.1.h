
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef BIONET_NODE_H
#define BIONET_NODE_H

/**
 * @file bionet-node.h 
 * Functions for dealing with nodes
 */

#include <stdint.h>
#include <time.h>

#include <sys/time.h>

#include <glib.h>

#include "bionet-util.h"


/**
 * @brief Create a new node
 *
 * @param[in] hab The HAB which the node shall belong to
 * @param[in] node_id ID of the new node 
 *
 * @return Pointer to the new node
 * @retval NULL Failure
 */
bionet_node_t* bionet_node_new(bionet_hab_t *hab, const char* node_id);


/**
 * @brief Get the Bionet qualified name of the Node
 *
 * Node name is of the format <HAB-Type>.<HAB-ID>.<Node-ID>
 *
 * @param[in] node The Node
 * @param[out] name Pointer to the buffer the name shall be written into
 * @param[in] name_len Length of the buffer pointed to by name
 *
 * @return Number of characters which would have been written to the buffer not 
 * including the terinating NULL 
 * @return -1 Error
 *
 * @note If the return value is greater than or equal to name_len the name has
 * been truncated. Suggested size for the buffer is 
 * 3*BIONET_NAME_COMPONENT_MAX_LEN. Check snprintf utility for more information.
 */
int bionet_node_get_name(const bionet_node_t * node,
			 char * name,
			 int name_len);


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
const char * bionet_node_get_id(const bionet_node_t *node);


/**
 * @brief Get the parent HAB of a node
 *
 * @param[in] node Pointer to a Node
 * 
 * @return Pointer to a HAB
 * @return NULL on failure
 */
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
int bionet_node_add_resource(bionet_node_t *node, bionet_resource_t *resource);


/**
 * @brief Get the number of resource on a node
 *
 * @param[in] node Node to query
 *
 * @retval -1 Failure
 * @retval >=0 Number of nodes
 */
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
int bionet_node_add_stream(bionet_node_t *node, bionet_stream_t *stream);


/**
 * @brief Get the number of streams for a node
 * 
 * @param[in] node Node to query
 *
 * @retval -1 Failure
 * @retval >=0 Number of streams
 */
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
bionet_stream_t *bionet_node_get_stream_by_id(const bionet_node_t *node, const char *stream_id);


/**
 * @brief Free a node
 *
 * As a side-effect all resources and streams will also be free'd
 *
 * @param[in] node Node to free
 */
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
int bionet_node_matches_habtype_habid_nodeid(const bionet_node_t *node, 
					     const char *hab_type, 
					     const char *hab_id, 
					     const char *node_id);


#endif //  BIONET_NODE_H

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
