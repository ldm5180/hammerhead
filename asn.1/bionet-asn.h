
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef BIONET_ASN_H
#define BIONET_ASN_H


#include <sys/time.h>

#include "Datapoint.h"

#include "ResourceFlavor.h"
#include "ResourceDataType.h"
#include "Resource.h"

#include "StreamDirection.h"
#include "Stream.h"

#include "Node.h"

#include "HardwareAbstractor.h"

#include "ResourceDatapointsQuery.h"
#include "ResourceDatapointsReply.h"

#include "C2H-Message.h"
#include "H2C-Message.h"

#include "BDM-C2S-Message.h"
#include "BDM-S2C-Message.h"

#include "BDM-Sync-Datapoints-Message.h"
#include "BDM-Sync-Metadata-Message.h"
#include "BDM-Sync-Message.h"
#include "BDM-Sync-Ack.h"

#include "BDMSyncRecord.h"

#include "ResourceRecord.h"

#include "bionet-util.h"




typedef struct {
    size_t size;
    void *buf;
} bionet_asn_buffer_t;

// useful as the "buffer acceptor" of der_encoder()
int bionet_accumulate_asn_buffer(const void *new_buffer, size_t new_size, void *buffer_as_voidp);




// 
// helper functions for dealing with ASN.1
// these ones here should probably be offered to the asn1c project as patches
//

// converts an ASN.1 GeneralizedTime variable to a struct timeval
// returns 0 on success, -1 on failure
int bionet_GeneralizedTime_to_timeval(const GeneralizedTime_t *gt, struct timeval *tv);


// converts a struct timeval to an ASN.1 GeneralizedTime
// returns 0 on success, -1 on failure
int bionet_timeval_to_GeneralizedTime(const struct timeval *tv, GeneralizedTime_t *gt);




// 
// helper functions for dealing with ASN.1
// these ones here are probably only useful to Bionet
//
// FIXME: some of these allocate a new object and return it, some take a pointer in and fill it int
//


int bionet_node_to_asnbuf(const bionet_node_t *node, bionet_asn_buffer_t *buf);


//!
//! \brief Makes a buffer containing a DER-encoded asn1c
//!     ResourceMetadata_t, from a bionet_resource_t
//!
//! \param resource The Bionet Resource to use as the source.
//!
//! \param buf The buffer to deposit the resulting ASN.1 message into.
//!
//! \return 0 on success, -1 on failure.
//!

int bionet_resource_metadata_to_asnbuf(bionet_resource_t *resource, bionet_asn_buffer_t *buf);


//!
//! \brief Makes a buffer containing a DER-encoded asn1c
//!     ResourceDatapoints_t, from a bionet_resource_t
//!
//! \param resource The Bionet Resource to use as the source.
//!
//! \param buf The buffer to deposit the resulting ASN.1 message into.
//!
//! \param dirty_only If true, send only dirty datapoints.
//!
//! \return 0 on success, -1 on failure.
//!

int bionet_resource_datapoints_to_asnbuf(bionet_resource_t *resource, bionet_asn_buffer_t *buf, int dirty_only);


//!
//! \brief Makes an asn1c Node_t from a bionet_node_t
//!
//! \param node The Bionet Node to use as the source.
//!
//! \param asn_node The asn1c Node_t to use as the destination.
//!     The asn_node must be allocated but not initialized.  It does not
//!     have to be zeroed.
//!
//! \return 0 on success, -1 on failure.
//!

int bionet_node_to_asn(const bionet_node_t *node, Node_t *asn_node);


//!
//! \brief Makes an asn1c HArdwareAbstractor_t from a bionet_hab_t
//!
//! \param hab The Bionet HAB to use as the source.
//!
//! \param asn_hab The asn1c HardwareAbstractor_t to use as the destination.
//!     The asn_hab must be allocated but not initialized.  It does not
//!     have to be zeroed.
//!
//! \return 0 on success, -1 on failure.
//!

int bionet_hab_to_asn(const bionet_hab_t * hab, HardwareAbstractor_t * asn_hab);


//!
//! \brief Makes an asn1c Resource_t from a bionet_resource_t
//!
//! \param node The Bionet Resource to use as the source.
//!
//! \return A pointer to the Resource_t on success, NULL on failure.
//!

Resource_t *bionet_resource_to_asn(bionet_resource_t *resource);


bionet_node_t *bionet_asn_to_node(const Node_t *asn_node);
bionet_node_t *bionet_asn_to_node_21(const Node_t *asn_node, bionet_hab_t *hab);


ResourceFlavor_t bionet_flavor_to_asn(bionet_resource_flavor_t flavor);
ResourceDataType_t bionet_datatype_to_asn(bionet_resource_data_type_t datatype);
Datapoint_t *bionet_datapoint_to_asn(bionet_datapoint_t *d);

bionet_resource_flavor_t bionet_asn_to_flavor(ResourceFlavor_t asn_flavor);
bionet_resource_data_type_t bionet_asn_to_datatype(ResourceDataType_t asn_datatype);
bionet_datapoint_t *bionet_asn_to_datapoint(Datapoint_t *asn_datapoint, bionet_resource_t *resource);

bionet_stream_direction_t bionet_asn_to_stream_direction(StreamDirection_t asn_direction);
StreamDirection_t bionet_stream_direction_to_asn(bionet_stream_direction_t direction);
Stream_t *bionet_stream_to_asn(const bionet_stream_t *stream);
bionet_stream_t *bionet_asn_to_stream(const Stream_t *asn_stream);




#endif // BIONET_ASN_H

