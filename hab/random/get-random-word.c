
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef WINDOWS
    #include <windows.h>
#endif

#include <glib.h>

#include "bionet-util.h"
#include "hardware-abstractor.h"

#include "random-hab.h"

#include "resource-ids.h"

extern int urandom_fd;


const char *get_random_word(void) {
    int num_resource_ids = sizeof(random_resource_ids) / sizeof(char*);
    int rnd;
    if (sizeof(rnd) != read(urandom_fd, &rnd, sizeof(rnd))) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Error reading from /dev/urandom: %m");
	return NULL;
    }
    return random_resource_ids[abs(rnd) % num_resource_ids];
}

