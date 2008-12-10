
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef BIONET_HAB_H
#define BIONET_HAB_H


#include <stdint.h>
#include <time.h>

#include <sys/time.h>

#include <glib.h>

#include "bionet-util.h"


struct bionet_hab {
    char *type;
    char *id;

    GSList *nodes;

    void *user_data;
};


/**
 * @file bionet-hab.h 
 * Functions for dealing with Bionet Hardware-Abstractor (HABs).
 */


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
bionet_hab_t *bionet_hab_new(const char *type, const char *id);


/**
 * @brief Set the type of an existing HAB 
 *
 * @param[in] hab Pointer to a HAB
 * @param[in] type Type of HAB
 * 
 * @retval 0 Success
 * @retval -1 Failure
 */
int bionet_hab_set_type(bionet_hab_t *hab, const char *type);


/**
 * @brief Set the ID of an existing HAB 
 *
 * @param[in] hab Pointer to a HAB
 * @param[in] id ID of HAB
 * 
 * @retval 0 Success
 * @retval -1 Failure
 */
int bionet_hab_set_id(bionet_hab_t *hab, const char *id);


/**
 * @brief Get pointer to a node of a HAB by its ID
 *
 * @param[in] hab Pointer to a HAB
 * @param[in] node_id ID of the node requested 
 *
 * @retval Valid node pointer on success
 * @retval NULL if node with that ID does not exist
 */
bionet_node_t *bionet_hab_get_node_by_id(bionet_hab_t *hab, 
					 const char *node_id);


/**
 * @brief Get the number of nodes in the HAB
 * 
 * @param[in] hab Pointer to a HAB
 * 
 * @retval -1 Invalid HAB pointer
 * @retval Number of nodes in the HAB on success
 */
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
bionet_node_t *bionet_hab_get_node_by_index(bionet_hab_t *hab, 
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
 *
 * @todo Node removed shall be returned instead of a success or failure code 
 */
int bionet_hab_remove_node_by_id(bionet_hab_t *hab, const char *node_id);


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
int bionet_hab_remove_all_nodes(bionet_hab_t *hab);


/**
 * @brief Free a HAB
 *
 * @param[in] hab Pointer to a HAB
 *
 * @note As a side-effect all associated nodes are removed and free'd
 */
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
int bionet_hab_matches_type_and_id(const bionet_hab_t *hab, 
				   const char *type, 
				   const char *id);


#endif //  BIONET_HAB_H


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
