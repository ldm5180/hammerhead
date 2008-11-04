
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bionet-asn.h"


int bionet_GeneralizedTime_to_timeval(const GeneralizedTime_t *gt, struct timeval *tv) {
    time_t r;
    int usec;

    r = asn_GT2time_prec(gt, &usec, 6, NULL, 0);
    if (r == -1) {
        return -1;
    }

    tv->tv_sec = r;
    tv->tv_usec = usec;

    return 0;
}


int bionet_timeval_to_GeneralizedTime(const struct timeval *tv, GeneralizedTime_t *gt) {
    struct tm tm;
    GeneralizedTime_t *result;

    if (gmtime_r(&tv->tv_sec, &tm) == NULL) {
        return -1;
    }

    result = asn_time2GT_frac(gt, &tm, tv->tv_usec, 6, 1);
    if (result == NULL) {
        return -1;
    }

    return 0;
}


int bionet_accumulate_asn_buffer(const void *new_buffer, size_t new_size, void *buffer_as_voidp) {
    bionet_asn_buffer_t *buffer = buffer_as_voidp;

    buffer->buf = realloc(buffer->buf, (buffer->size + new_size));
    if (buffer->buf == NULL) {
        return -1;
    }

    memcpy((buffer->buf + buffer->size), new_buffer, new_size);
    buffer->size += new_size;

    return new_size;
}

