
#ifndef BIONET_ASN_H
#define BIONET_ASN_H


#include <sys/time.h>

#include "ResourceFlavor.h"
#include "ResourceDataType.h"
#include "Resource.h"

#include "Node.h"


#include "C2H-Message.h"
#include "H2C-Message.h"




// maximum supported on-the-wire message size
// FIXME: it'd be nice if asn1c could tell me this, instead i'll just make a number up
#define BIONET_MSG_MAX_LEN (1024)




typedef struct {
    size_t size;
    void *buf;
} bionet_asn_buffer_t;




// 
// helper functions for dealing with ASN.1
// FIXME: some of this should probably be offered to the asn1c project as patches
//

// converts an ASN.1 GeneralizedTime variable to a struct timeval
// returns 0 on success, -1 on failure
int bionet_GeneralizedTime_to_timeval(const GeneralizedTime_t *gt, struct timeval *tv);


// converts a struct timeval to an ASN.1 GeneralizedTime
// returns 0 on success, -1 on failure
int bionet_timeval_to_GeneralizedTime(const struct timeval *tv, GeneralizedTime_t *gt);


// useful as the "buffer acceptor" of der_encoder()
int bionet_accumulate_asn_buffer(const void *new_buffer, size_t new_size, void *buffer);




#endif // BIONET_ASN_H

