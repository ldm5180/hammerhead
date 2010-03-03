
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef BIONET_HAB_H
#define BIONET_HAB_H


#include "libbionet-util-decl.h"

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
 * @retval 0 Insecure
 * @retval 1 Secure
 *
 * @note This function does not make sense for HABs, only Bionet Clients.
 */
BIONET_UTIL_API_DECL
int bionet_hab_is_secure(const bionet_hab_t *hab);


/**
 * @brief Set the security flag on a HAB
 * 
 * @param[in] hab The HAB
 * @param[in] is_secure 0 = Insecure, 1 = Secure
 * 
 * @note This function is used exclusively by the Bionet Client library.
 */
BIONET_UTIL_API_DECL
void bionet_hab_set_secure(bionet_hab_t *hab, int is_secure);

/**
 * @brief Get pointer to a bdm of a HAB by its ID
 *
 * @param[in] hab Pointer to a HAB
 * @param[in] bdm_id ID of the bdm requested 
 *
 * @retval Valid bdm pointer on success
 * @retval NULL if bdm with that ID does not exist
 */
BIONET_UTIL_API_DECL
bionet_bdm_t *bionet_hab_get_bdm_by_id(bionet_hab_t *hab, 
					 const char *bdm_id);


/**
 * @brief Get the number of bdms in the HAB
 * 
 * @param[in] hab Pointer to a HAB
 * 
 * @retval -1 Invalid HAB pointer
 * @retval Number of bdms in the HAB on success
 */
BIONET_UTIL_API_DECL
int bionet_hab_get_num_bdms(const bionet_hab_t *hab);


/**
 * @brief Get a bdm by its index in the HABs list
 * 
 * Useful for iterating over all the bdms in a HAB
 *
 * @param[in] hab Pointer to a HAB
 * @param[in] index Index of the bdm desired
 *
 * @retval NULL Invalid HAB pointer or index is greater than number of bdms
 * @retval Valid bdm pointer on success
 */
BIONET_UTIL_API_DECL
bionet_bdm_t *bionet_hab_get_bdm_by_index(bionet_hab_t *hab, 
					    unsigned int index);


/**
 * @brief Add a bdm to the HAB
 *
 * @param[in] hab Pointer to a HAB
 * @param[in] bdm Pointer to a bdm
 *
 * @retval 0 Success
 * @retval -1 Failure
 *
 * @note The BDM memory is not managed by the hab. The hab only tracks BDMs
 * for cross-linking purposes
 */
BIONET_UTIL_API_DECL
int bionet_hab_add_bdm(bionet_hab_t *hab, const bionet_bdm_t *bdm);


/**
 * @brief Remove a specific bdm from a HAB
 * 
 * @param[in] hab Pointer to a HAB
 * @param[in] bdm_id ID of bdm to remove
 *
 * @return 0 Success
 * @return -1 Failure
 *
 * @note BDM is not free'd. The memory is mananged in the cache
 */
BIONET_UTIL_API_DECL
bionet_bdm_t * bionet_hab_remove_bdm_by_id(bionet_hab_t *hab, const char *bdm_id);


/**
 * @brief Remove all bdms from a HAB
 *
 * @param[in] hab Pointer to a HAB
 *
 * @return 0 Success
 * @return -1 Failure
 *
 * @note The BDMs are not freed. The memory is still owed by the cache
 */
BIONET_UTIL_API_DECL
int bionet_hab_remove_all_bdms(bionet_hab_t *hab);


/**
 * @brief Get the id of the BDM that recorded this hab
 *
 * @param[in] hab Pointer to HAB
 *
 * @return Pointer to string that is valid until the hab is free'd
 */ 
BIONET_UTIL_API_DECL
const char * bionet_hab_get_recording_bdm(bionet_hab_t *hab);

/**
 * @brief Set the ID of the BDM that recorded this hab
 *
 * @param[in] hab Pointer to HAB
 * @param[in] bdm_id ID of BDM
 *
 * @return Pointer to string that is valid until the hab is free'd
 */ 
BIONET_UTIL_API_DECL
void bionet_hab_set_recording_bdm(bionet_hab_t *hab, const char * bdm_id);

#ifdef __cplusplus
}
#endif

#endif //  BIONET_HAB_H


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
