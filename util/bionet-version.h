
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

/**
 * @brief Print the bionet version to a stream
 *
 * @param[in] stream The stream to print to.
 */
BIONET_UTIL_API_DECL
void print_bionet_version(FILE *stream);


/**
 * @brief Get the Bionet version string
 *
 * @param[out] version Pointer to the pointer to store the version string in
 *
 * @note The the version pointer points to a NULL pointer then a memory
 * is allocated for the string and ust be freed by the caller.
 */
BIONET_UTIL_API_DECL
void bionet_version_get(char ** version);

#ifdef __cplusplus
}
#endif


#endif /* BIONET_VERSION_H */
