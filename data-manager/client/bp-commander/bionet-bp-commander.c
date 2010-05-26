
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include "bdm-client.h"


int main(int argc, char *argv[]) {
    char *src_eid;
    char *resource_name;
    char *value;
    char *dest_eid;

    int r;

    if (argc != 5) {
        fprintf(stderr, "usage: bionet-bp-commander Source_EID Resource Value Dest_EID\n");
        exit(1);
    }

    src_eid = argv[1];
    resource_name = argv[2];
    value = argv[3];
    dest_eid = argv[4];


    r = bionet_bp_start(src_eid);
    if (r != 0) {
        return 1;
    } 

    r = bionet_bp_set_resource_by_name(
        dest_eid,         // the EID of the proxy
        100,              // bundle TTL in seconds
        resource_name,
        value
    );
    if (r != 0) {
        return 1;
    } 

    bionet_bp_stop();


    return 0;
}

