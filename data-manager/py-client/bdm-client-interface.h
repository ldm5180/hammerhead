
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


/**
 * Get a specific HAB from the HAB list
 *
 * @param[in] hab_list Array of HABs
 * @param[in] index Index in the array of HABs
 *
 * @return Ptr to a bdm_hab_t or NULL if the index is beyond the bounds of array
 */
bionet_hab_t * bionet_get_hab(GPtrArray *hab_list, int index);

// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
