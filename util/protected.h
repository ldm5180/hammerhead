
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#ifndef BIONET_UTIL_PROTECTED_H
#define BIONET_UTIL_PROTECTED_H

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


int bionet_value_check_epsilon(const bionet_value_t * original_val, 
			       const void * content, 
			       const bionet_epsilon_t * epsilon, 
			       bionet_resource_data_type_t data_type);

int bionet_value_check_epsilon_by_value(const bionet_value_t * original_val, 
					const bionet_value_t * new_val, 
					const bionet_epsilon_t * epsilon, 
					bionet_resource_data_type_t data_type);


int bionet_value_check_delta(const struct timeval * original_tv,
			     const struct timeval * new_tv,
			     const struct timeval * delta);

#endif /* BIONET_UTIL_PROTECTED_H */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
