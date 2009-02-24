
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include "hardware-abstractor.h"
#include "streamy-hab.h"


void make_outgoing_node(bionet_hab_t *this_hab) {
    bionet_node_t *node;

    DIR *d;
    struct dirent *de;
                      

    node = bionet_node_new(this_hab, "outgoing");
    if (node == NULL) {
        fprintf(stderr, "error making 'outgoing' Node\n");
        exit(1);
    }

    d = opendir(".");
    if (d == NULL) {
        fprintf(stderr, "error opening '.': %s\n", strerror(errno));
        exit(1);
    }

    while ((de = readdir(d)) != NULL) {
        bionet_stream_t *stream;
        char *filename;
        char *p;

        if (de->d_type != DT_REG) continue;

        filename = strdup(de->d_name);
        if (filename == NULL) {
            fprintf(stderr, "out of memory!");
            exit(1);
        }

        // make the filename into an acceptable stream id
        for (p = de->d_name; *p != (char)0; p++) {
            if (isalnum(*p)) continue;
            if (*p == '-') continue;
            *p = '-';
        }

        stream = bionet_stream_new(node, de->d_name, BIONET_STREAM_DIRECTION_PRODUCER, "file");
        if (stream == NULL) {
            fprintf(stderr, "error making '%s' Stream\n", de->d_name);
            exit(1);
        }

        bionet_stream_set_user_data(stream, filename);

        bionet_node_add_stream(node, stream);
    }

    bionet_hab_add_node(this_hab, node);

    hab_report_new_node(node);
}

