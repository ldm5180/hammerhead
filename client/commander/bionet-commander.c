
// Copyright (c) 2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <getopt.h>

#include <glib.h>

#include "bionet.h"
#include "bionet-util.h"


static char *resource_name;
static char *value;


static void cb_new_hab(bionet_hab_t *hab) {
    int r;

    printf("HAB %s showed up, setting %s to %s\n", bionet_hab_get_name(hab), resource_name, value);
    r = bionet_set_resource_by_name(resource_name, value);
    if (r != 0) {
        fprintf(stderr, "the HAB did not accept the command, says Bionet\n");
        exit(1);
    }
    sleep(1); // FIXME: do i need this to give CAL time to send the message?
    exit(0);
}


int main(int argc, char *argv[]) {
    int bionet_fd;
    char *hab_type;
    char *hab_id;

    struct timeval start, now, timeout;
    int seconds_to_wait = 5;

    int r;


    if (argc != 3) {
        fprintf(stderr, "usage: bionet-commander Resource Value\n");
        exit(1);
    }

    resource_name = argv[1];
    value = argv[2];

    r = bionet_split_resource_name(
        resource_name,
        &hab_type,
        &hab_id,
        NULL,
        NULL
    );
    if (r != 0) {
        fprintf(stderr, "error splitting Resource Name %s\n", resource_name);
        exit(1);
    }


    bionet_register_callback_new_hab(cb_new_hab);

    {
        char hab_name[2*BIONET_NAME_COMPONENT_MAX_LEN];
        int r;

        r = snprintf(hab_name, sizeof(hab_name), "%s.%s", hab_type, hab_id);
        if (r >= sizeof(hab_name)) {
            fprintf(stderr, "HAB name of '%s' too long!\n", resource_name);
            exit(1);
        }

        bionet_subscribe_hab_list_by_name(hab_name);
    }


    bionet_fd = bionet_connect();
    if (bionet_fd < 0) {
        fprintf(stderr, "error connecting to Bionet");
        exit(1);
    }


    timeout.tv_sec = seconds_to_wait;
    timeout.tv_usec = 0;

    r = gettimeofday(&start, NULL);
    if (r != 0) {
        fprintf(stderr, "error with gettimeofday: %s\n", strerror(errno));
        exit(1);
    }

    while (1) {
        r = bionet_read_with_timeout(&timeout);
        if (r != 0) {
            fprintf(stderr, "error/timeout reading from bionet\n");
            exit(1);
        }

        r = gettimeofday(&now, NULL);
        if (r != 0) {
            fprintf(stderr, "error with gettimeofday: %s\n", strerror(errno));
            exit(1);
        }

        timeout.tv_sec = seconds_to_wait - (now.tv_sec - start.tv_sec);
        timeout.tv_usec = (now.tv_usec - start.tv_usec);
        if (timeout.tv_usec < 0) {
            timeout.tv_sec --;
            timeout.tv_usec += 1000 * 1000;
        }
        if (timeout.tv_sec < 0) {
            fprintf(stderr, "timeout, HAB never showed up\n");
            exit(1);
        }
    }


    return 0;
}

