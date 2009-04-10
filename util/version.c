
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include "bionet-version.h"

void print_bionet_version(FILE *stream) {
    fprintf(stream, "%s%s\n", BIONET_VERSION, BIONET_BUILD_STR);
} /* print_bionet_version() */
