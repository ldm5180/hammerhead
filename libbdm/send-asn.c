
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <unistd.h>

#include "bdm-client.h"

extern int bdm_fd;

int bdm_send_asn(const void *buffer, size_t size, void *unused) {
    return write(bdm_fd, buffer, size);
}

