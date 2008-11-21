

//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <unistd.h>

#include "bdm-client.h"

extern int bdm_fd;

int bdm_send_asn(const void *buffer, size_t size, void *unused) {
    return write(bdm_fd, buffer, size);
}

