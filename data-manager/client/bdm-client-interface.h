
/**
 * Get a specific HAB from the HAB list
 *
 * @param[in] hab_list Array of HABs
 * @param[in] index Index in the array of HABs
 *
 * @return Ptr to a bdm_hab_t or NULL if the index is beyond the bounds of array
 */
bdm_hab_t * bdm_get_hab(GPtrArray *hab_list, int index);

// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
