
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <check.h>

#include "check-common.h"
#include "check-asn1.h"
#include "bionet-asn.h"


typedef struct {
    void * buf;
    size_t size;
} blob_t;

static int send_message_to_buffer(const void *buffer, size_t size, void * config_void) {
    blob_t * pb = (blob_t*)config_void;

    pb->buf = realloc(pb->buf, pb->size + size);
    uint8_t *p = (uint8_t*)pb->buf + pb->size;
    pb->size += size;

    memcpy(p, buffer, size);

    return size;
} /* send_message_to_buffer() */


START_TEST (test_asn_encode_float) {

    int r;
    float f1 = 108E-19;
    Datapoint_t *asn_datapoint, *asn_datapoint_read;
    struct timeval now = {1245865364, 100};

    asn_datapoint = (Datapoint_t *)calloc(1, sizeof(Datapoint_t));
    fail_if(NULL == asn_datapoint, "Out of memory!");
    asn_datapoint_read = (Datapoint_t *)calloc(1, sizeof(Datapoint_t));
    fail_if(NULL == asn_datapoint_read, "Out of memory!");


    r = bionet_timeval_to_GeneralizedTime(&now, &asn_datapoint->timestamp);
    fail_unless(r == 0, "Error encoding time");

    asn_datapoint->value.present = Value_PR_real;
    asn_datapoint->value.choice.real = f1;


    /* DER Encode */
    asn_enc_rval_t asn_r;
    blob_t blob = {0,0};
    asn_r = der_encode(&asn_DEF_Datapoint, asn_datapoint, send_message_to_buffer, &blob);
    fail_if(asn_r.encoded == -1, 
        "send_sync_datapoints(): error with der_encode(): %m");

    /* DER Decoder */
    asn_dec_rval_t rval;
    rval = ber_decode(NULL, &asn_DEF_Datapoint, (void**)&asn_datapoint_read, 
                      blob.buf, blob.size);
    fail_unless(rval.code == RC_OK, "Decode not OK");
    
    fail_unless(f1 == asn_datapoint_read->value.choice.real);

} END_TEST /* test_asn_encode_float */


void bionet_asn1_test_suite(Suite *s)
{
    TCase *tc = tcase_create("bionet_asn1_test_suite()");
    suite_add_tcase(s, tc);

    tcase_add_test(tc, test_asn_encode_float);

    return;
} /* libutil_hab_tests_suite() */


// vim: ts=8 sw=4 sta expandtab
// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// indent-tabs-mode: nil
// End:
