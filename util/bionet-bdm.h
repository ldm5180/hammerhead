
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
 * @brief Free a BDM
 *
 * @param[in] bdm Pointer to a BDM
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


/**
 * @brief Add a destruction notifier
 *
 * Each destruction notifier will be called in the order they
 * were added when bionet_bdm_free() is called.
 *
 * @param[in] bdm BDM to add the destructor for
 * @param[in] destructor The destructor to run.
 * @param[in] user_data User data to pass into the destructor.
 *
 * @retval 0 Successfully added.
 * @retval 1 Failed to add.
 */
BIONET_UTIL_API_DECL
int bionet_bdm_add_destructor(bionet_bdm_t * bdm, 
			      void (*destructor)(bionet_bdm_t * bdm, void * user_data),
			      void * user_data);


/**
 * @brief Increment the reference count
 *
 * This function is used by wrappers of this interface. It is not
 * needed for writing C, but is for SWIG-generated Python so that
 * garbage collection works properly.
 *
 * @param[in] bdm Bdm to increment the reference count for
 */
BIONET_UTIL_API_DECL
void bionet_bdm_increment_ref_count(bionet_bdm_t * bdm);


/**
 * @brief Get the reference count
 *
 * This function is used by wrappers of this interface. It is not
 * needed for writing C, but is for SWIG-generated Python so that
 * garbage collection works properly.
 *
 * @param[in] bdm Bdm to get the reference count for
 *
 * @return Number of references currently held.
 */
BIONET_UTIL_API_DECL
unsigned int bionet_bdm_get_ref_count(bionet_bdm_t * bdm);


#endif //  BIONET_BDM_H


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
