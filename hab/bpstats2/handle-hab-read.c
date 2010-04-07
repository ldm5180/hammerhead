
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include "bpstats2-hab.h"

/* Bionet */
int bionet_fd;
int require_security = 0;
char * security_dir = NULL;
char *hab_type = "bpstats2-hab";
char *hab_id = NULL;
bionet_hab_t *hab = NULL;

//
// Thread for calling hab_read whenever bionet_fd is ready.
//
pthread_t handle_hab_read_thread;
void *handle_hab_read(void *unused)
{
    fd_set rfds;
    int rc;
  

    while(keep_running) {
        FD_ZERO(&rfds);
        FD_SET(bionet_fd, &rfds);

        rc = select(bionet_fd + 1, &rfds, NULL, NULL, NULL);
        if(rc < 0) {
            g_error("Error in handle_hab_read select(): %s", strerror(errno));
        }
        if(rc == 0) continue;

        // If here: should call hab_read().
        if(pthread_mutex_lock(&doing_bionet_mutex) < 0) {
            g_error("handle_hab_read couldn't lock Bionet mutex: %s", 
                strerror(errno));
        }
        hab_read();
        if(pthread_mutex_unlock(&doing_bionet_mutex) < 0) {
            g_error("handle_hab_read couldn't unlock Bionet mutex: %s", 
                strerror(errno));
        }
    }
    g_debug("handle_hab_read thread returning.");
    return NULL;
}
