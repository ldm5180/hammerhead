
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef BDM_ASN_H
#define BDM_ASN_H

#include "libbionet-asn-decl.h"

#include <sys/time.h>

#include "bionet-util.h"

#include "BDM-HardwareAbstractor.h"
#include "BDM-Resource.h"

BIONET_ASN_API_DECL
int bdm_new_hab_to_asnbuf(
        const char * hab_type, 
        const char * hab_id,
        long entry_seq, 
        const struct timeval *timestamp, 
        const char * bdm_id,
        bionet_asn_buffer_t *buf) ;

BIONET_ASN_API_DECL
int bdm_lost_hab_to_asnbuf(
        const char * hab_type, 
        const char * hab_id,
        long entry_seq, 
        const struct timeval *timestamp, 
        const char * bdm_id,
        bionet_asn_buffer_t *buf) ;

BIONET_ASN_API_DECL
int bdm_new_node_to_asnbuf(
        bionet_node_t *node,
        bionet_event_t *event,
        long entry_seq,
        bionet_asn_buffer_t *buf) ;

BIONET_ASN_API_DECL
int bdm_lost_node_to_asnbuf(
        bionet_node_t * node,
        bionet_event_t * event,
        long entry_seq, 
        bionet_asn_buffer_t *buf);

BIONET_ASN_API_DECL
int bdm_resource_metadata_to_asnbuf(
        bionet_resource_t *resource,
        long entry_seq,
        bionet_asn_buffer_t *buf);


BIONET_ASN_API_DECL
int bdm_resource_datapoints_to_asnbuf(
        bionet_resource_t *resource,
        long entry_seq,
        bionet_asn_buffer_t *buf);

BIONET_ASN_API_DECL
bionet_datapoint_t *bdm_publish_asn_to_datapoint(
        BDMPublishDatapoint_t *asn_datapoint,
        bionet_resource_t *resource,
        long * ret_seq);

BIONET_ASN_API_DECL
DataManager_t * bionet_bdm_to_asn(bionet_bdm_t * bdm);


#endif /* BDM_ASN_H */
