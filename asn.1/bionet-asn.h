
#ifndef BIONET_ASN_H
#define BIONET_ASN_H


#include <sys/time.h>

#include "Datapoint.h"

#include "ResourceFlavor.h"
#include "ResourceDataType.h"
#include "Resource.h"

#include "Node.h"


#include "C2H-Message.h"
#include "H2C-Message.h"


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

int bionet_node_to_asnbuf(const bionet_node_t *node, bionet_asn_buffer_t *buf);

bionet_node_t *bionet_asn_to_node(const Node_t *asn_node);


ResourceFlavor_t bionet_flavor_to_asn(bionet_resource_flavor_t flavor);
ResourceDataType_t bionet_datatype_to_asn(bionet_resource_data_type_t datatype);
Datapoint_t *bionet_datapoint_to_asn(bionet_datapoint_t *d);

bionet_resource_flavor_t bionet_asn_to_flavor(ResourceFlavor_t asn_flavor);
bionet_resource_data_type_t bionet_asn_to_datatype(ResourceDataType_t asn_datatype);
bionet_datapoint_t *bionet_asn_to_datapoint(Datapoint_t *asn_datapoint, bionet_resource_t *resource);




#endif // BIONET_ASN_H

