
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

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




const char *get_random_word(void) {
    int num_resource_ids = sizeof(random_resource_ids) / sizeof(char*);
    return random_resource_ids[rand() % num_resource_ids];
}

