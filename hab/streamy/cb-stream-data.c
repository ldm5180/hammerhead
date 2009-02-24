
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "hardware-abstractor.h"
#include "streamy-hab.h"


void cb_stream_data(const char *client_id, bionet_stream_t *stream, const void *data, unsigned int size) {
    const char *filename = bionet_stream_get_local_name(stream);
    FILE *f;
    int r;

    f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "error opening '%s' for append: %s\n", filename, strerror(errno));
        return;
    }

    r = fwrite(data, 1, size, f);
    fclose(f);
    if (r != size) {
        fprintf(stderr, "error appending to '%s'\n", filename);
        return;
    }
}

