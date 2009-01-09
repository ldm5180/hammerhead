
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <string.h>

#include <glib.h>

#include "hardware-abstractor.h"

#include "libhab-internal.h"
#include "bionet-asn.h"


int hab_publish_stream(const bionet_stream_t *stream, const void *buf, size_t count) {
    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_INFO, "hab_publish_stream(): should publish %d bytes on %s", (int)count, bionet_stream_get_local_name(stream));
    return 0;
}


