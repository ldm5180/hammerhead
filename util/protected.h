
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#ifndef BIONET_UTIL_PROTECTED_H
#define BIONET_UTIL_PROTECTED_H

#include "bionet-util.h"

/**
 * @brief Checks if a Resource has any dirty Datapoints.
 *
 * Dirty Datapoints are ones that have data which hasn't been reported to
 * Bionet yet.
 *
 * @param[in] resource The Resource to test for dirtiness.
 *
 * @retval True (non-zero) - Dirty
 * @retval False (zero) - Not dirty
 */
BIONET_UTIL_API_DECL
int bionet_resource_is_dirty(const bionet_resource_t *resource);


/**
 * @brief Makes a Resource clean, by making all its Datapoints clean.
 *
 * @param[in] resource The Resource to clean.
 */
BIONET_UTIL_API_DECL
void bionet_resource_make_clean(bionet_resource_t *resource);


/**
 * @brief Check if the resource is being persisted
 *
 * @param[in] resource The resource to check.
 *
 * @retval 0 Not persisted
 * @retval 1 Persisted
 */
BIONET_UTIL_API_DECL
int bionet_resource_is_persisted(const bionet_resource_t * resource);

/**
 * @brief Persist the value of any datapoint set to this resource
 *
 * Using local storage, make the value of the datapoint persistent. This allows
 * the resource to have the same value next time the HAB is started.
 *
 * @param[in] resource The Resource
 * @param[in] persist_dir Directory containing persistence files.
 *
 * @retval 0 Success. Resource's value will be persisted each time a datapoint
 * is published for this resource.
 * @retval 1 Failure. This resource's value will not be persisted.
 *
 * @note If the resource does not yet have a datapoint, after calling this function
 * it will have a datapoint containing the current time and the last value persisted
 * by this resource if it has ever been persisted before.
 *
 * @note This function does not do the persisting. It only marks the resource as a
 * resource which requires persistence. Persisting is done as part of publishing using
 * hab_report_datapoints() or hab_report_new_node()
 */
BIONET_UTIL_API_DECL
int bionet_resource_persist(bionet_resource_t * resource, char * persist_dir);


/**
 * @brief Write the value of the resource to a persistent file
 * 
 * @param[in] resource Resource of which to write the value
 * @param[in] persist_dir Directory containing persistence files.
 *
 * @retval 0 Success
 * @retval 1 Failure
 */
BIONET_UTIL_API_DECL
int bionet_resource_write_persist(bionet_resource_t * resource, char * persist_dir);


/**
 * @brief Check to see if the epsilon as been exceeded.
 *
 * @param[in] original_val Value published previously
 * @param[in] content New value to be published
 * @param[in] epsilon Difference between new and previous before a publish is needed
 * @param[in] data_type Type to use for content pointer
 *
 * @retval 0 Not exceeded, no publish needed
 * @retval 1 Exceeded. Publish needed.
 */
int bionet_value_check_epsilon(const bionet_value_t * original_val, 
			       const void * content, 
			       const bionet_epsilon_t * epsilon, 
			       bionet_resource_data_type_t data_type);


/**
 * @brief Check to see if the epsilon as been exceeded.
 *
 * @param[in] original_val Value published previously
 * @param[in] new_val New value to be published
 * @param[in] epsilon Difference between new and previous before a publish is needed
 * @param[in] data_type Type to use for value pointers
 *
 * @retval 0 Not exceeded, no publish needed
 * @retval 1 Exceeded. Publish needed.
 */
int bionet_value_check_epsilon_by_value(const bionet_value_t * original_val, 
					const bionet_value_t * new_val, 
					const bionet_epsilon_t * epsilon, 
					bionet_resource_data_type_t data_type);


/**
 * @brief Check to see if the epsilon as been exceeded.
 *
 * @param[in] original_tv Timestamp published previously
 * @param[in] new_tv New timestamp to be published
 * @param[in] delta Difference between new and previous before a publish is needed
 *
 * @retval 0 Not exceeded, no publish needed
 * @retval 1 Exceeded. Publish needed.
 */
int bionet_value_check_delta(const struct timeval * original_tv,
			     const struct timeval * new_tv,
			     const struct timeval * delta);


/**
 * @brief Get the array of habs for this BDM
 *
 * Only needed by BDM
 */
BIONET_UTIL_API_DECL
GPtrArray *  bionet_bdm_get_hab_list(bionet_bdm_t * bdm);

/**
 * @brief Set the UUID of an existing Node 
 *
 * @param[in] node Pointer to a Node
 * @param[in] uuid Pointer to array of BDM_UUID_LEN bytes
 * 
 */
BIONET_UTIL_API_DECL
void bionet_node_set_uid(const bionet_node_t *node, const uint8_t uuid[BDM_UUID_LEN]);

/**
 * @brief Sort the resources of the node
 *
 * @param[in] node The node with the resorces to sort
 * @param[in] cmp Comparator function to use. a and b are both void pointers to
 * a bionet_resource_t. The function should return a negative integer if a
 * comes before b, 0 if a and b are equal, and a positive value if a sorts
 * after b. If the pointer is NULL, then a lexagraphic sort is used.
 */
BIONET_UTIL_API_DECL
void bionet_node_sort_resources(bionet_node_t *node,
        int (*cmp)(void * a, void * b));

/**
 * @brief Make a uid from the node
 *
 * @note After called, the node's resources may be reordered
 *
 * @param[in] node The node to calculate the UID from
 * @param[out] uid Buffer to populate with a uid
 */
int db_make_node_guid(
    bionet_node_t * node,
    uint8_t uid[BDM_UUID_LEN]);

/**
 * @brief Compare two datapoints
 *
 * @param[in] Pointer a to bionet_datapoint_t
 * @param[in] Pointer b to bionet_datapoint_t
 *
 * @return -1, 0, or 1 if a is less than, equal to or greater than b
 */ 
int bionet_datapoint_cmp(const void *a, const void *b);

#endif /* BIONET_UTIL_PROTECTED_H */



// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
