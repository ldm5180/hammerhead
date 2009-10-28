
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#ifndef BDM_ASN_H
#define BDM_ASN_H

#include "bionet-util.h"

int bdm_resource_metadata_to_asnbuf(
        bdm_t * bdm,
        bionet_resource_t *resource,
        bionet_asn_buffer_t *buf);


int bdm_resource_datapoints_to_asnbuf(
        bdm_t * bdm,
        bionet_resource_t *resource,
        bionet_asn_buffer_t *buf);

#endif // BDM_ASN_H

