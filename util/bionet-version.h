
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef BIONET_VERSION_H
#define BIONET_VERSION_H

#include <stdio.h>

#include "libbionet-util-decl.h"

#ifdef __cplusplus
extern "C" {
#endif


BIONET_UTIL_API_DECL
void print_bionet_version(FILE *stream);

BIONET_UTIL_API_DECL
void bionet_version_get(char ** version);

#ifdef __cplusplus
}
#endif


#endif /* BIONET_VERSION_H */
