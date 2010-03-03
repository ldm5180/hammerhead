
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef BIONET_BDM_H
#define BIONET_BDM_H


#include "libbionet-util-decl.h"

/**
 * @file bionet-bdm.h 
 * Functions for dealing with Bionet Data Manager objects (BDMs).
 */


#define BDM_TOPIC_MAX_LEN (4*BIONET_NAME_COMPONENT_MAX_LEN + 4)


/**
 * @brief Obtain a pointer to a new BDM
 *
 * @param[in] id ID of BDM or NULL. If NULL ID is set to the hostname
 *
 * @return Valid pointer on success
 * @return NULL on failure
 */
BIONET_UTIL_API_DECL
bionet_bdm_t *bionet_bdm_new(const char *id);

/**
 * @brief Get the ID of an existing BDM 
 *
 * @param[in] bdm Pointer to a BDM
 * 
 * @return BDM-ID string
 * @return NULL on failure
 *
 * @note Do not free the returned pointer
 */
BIONET_UTIL_API_DECL
const char * bionet_bdm_get_id(const bionet_bdm_t *bdm);


/**
 * @brief Get pointer to a hab of a BDM by its name
 *
 * @param[in] bdm Pointer to a BDM
 * @param[in] hab_name Name of the hab requested 
 *
 * @retval Valid hab pointer on success
 * @retval NULL if hab with that ID does not exist
 */
BIONET_UTIL_API_DECL
bionet_hab_t *bionet_bdm_get_hab_by_name(bionet_bdm_t *bdm, 
					 const char *hab_name);


/**
 * @brief Get pointer to a hab of a BDM by its Type.Name
 *
 * @param[in] bdm Pointer to a BDM
 * @param[in] hab_type type of the hab requested 
 * @param[in] hab_id ID of the hab requested 
 *
 * @retval Valid hab pointer on success
 * @retval NULL if hab with that ID does not exist
 */
BIONET_UTIL_API_DECL
bionet_hab_t *bionet_bdm_get_hab_by_type_id(
        bionet_bdm_t *bdm, const char *hab_type, const char *hab_id);


/**
 * @brief Get the number of habs in the BDM
 * 
 * @param[in] bdm Pointer to a BDM
 * 
 * @retval -1 Invalid BDM pointer
 * @retval Number of habs in the BDM on success
 */
BIONET_UTIL_API_DECL
int bionet_bdm_get_num_habs(const bionet_bdm_t *bdm);


/**
 * @brief Get a hab by its index in the BDMs list
 * 
 * Useful for iterating over all the habs in a BDM
 *
 * @param[in] bdm Pointer to a BDM
 * @param[in] index Index of the hab desired
 *
 * @retval NULL Invalid BDM pointer or index is greater than number of habs
 * @retval Valid hab pointer on success
 */
BIONET_UTIL_API_DECL
bionet_hab_t *bionet_bdm_get_hab_by_index(bionet_bdm_t *bdm, 
					    unsigned int index);


/**
 * @brief Add a hab to the BDM
 *
 * @param[in] bdm Pointer to a BDM
 * @param[in] hab Pointer to a hab
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
BIONET_UTIL_API_DECL
int bionet_bdm_add_hab(bionet_bdm_t *bdm, const bionet_hab_t *hab);


/**
 * @brief Remove a specific hab from a BDM
 * 
 * @param[in] bdm Pointer to a BDM
 * @param[in] hab_type Type of hab to remove
 * @param[in] hab_id ID of hab to remove
 *
 * @return 0 Success
 * @return -1 Failure
 *
 * @note Hab's memory is still owned by the cache
 */
BIONET_UTIL_API_DECL
bionet_hab_t * bionet_bdm_remove_hab_by_type_id(bionet_bdm_t *bdm, const char * hab_type, const char *hab_id);


/**
 * @brief Remove all habs from a BDM
 *
 * @param[in] bdm Pointer to a BDM
 *
 * @return 0 Success
 * @return -1 Failure
 *
 * @note All habs are still owned by the cache
 */
BIONET_UTIL_API_DECL
int bionet_bdm_remove_all_habs(bionet_bdm_t *bdm);


/**
 * @brief Free a BDM
 *
 * @param[in] bdm Pointer to a BDM
 *
 * @note All habs still exist in the cache
 */
BIONET_UTIL_API_DECL
void bionet_bdm_free(bionet_bdm_t *bdm);


/**
 * @brief Checks if a BDM matches a BDM-Type and BDM-ID pair.
 *
 * @param[in] bdm The BDM to test
 * @param[in] id The BDM-ID to compare
 *
 * @return 0 Failed match
 * @return 1 Successful match
 *
 * @note The wildcard "*" matches any string.
 */
BIONET_UTIL_API_DECL
int bionet_bdm_matches_id(const bionet_bdm_t *bdm, 
				   const char *id);


/**
 * @brief Set the user-data annotation of a BDM
 *
 * @param[in] bdm The BDM
 * @param[in] user_data The data to annotate the BDM with.
 *
 * @note If the user sets the user data, then the user is 
 *       responsible for freeing the it and setting it to 
 *       NULL before the bdm is free'd.
 */
BIONET_UTIL_API_DECL
void bionet_bdm_set_user_data(bionet_bdm_t *bdm, const void *user_data);


/**
 * @brief Get the user-data annotation of a BDM
 *
 * @param[in] bdm The BDM
 *
 * @return The user_data pointer, or NULL if none has been set.
 */
BIONET_UTIL_API_DECL
void *bionet_bdm_get_user_data(const bionet_bdm_t *bdm);


/**
 * @brief Check if communicating with the BDM over a secure connection
 * 
 * @param[in] bdm The BDM
 * 
 * @retval 0 Insecure
 * @retval 1 Secure
 *
 * @note This function does not make sense for BDMs, only Bionet Clients.
 */
BIONET_UTIL_API_DECL
int bionet_bdm_is_secure(const bionet_bdm_t *bdm);


/**
 * @brief Set the security flag on a BDM
 * 
 * @param[in] bdm The BDM
 * @param[in] is_secure 0 = Insecure, 1 = Secure
 * 
 * @note This function is used exclusively by the Bionet Client library.
 */
BIONET_UTIL_API_DECL
void bionet_bdm_set_secure(bionet_bdm_t *bdm, int is_secure);


#endif //  BIONET_BDM_H


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
