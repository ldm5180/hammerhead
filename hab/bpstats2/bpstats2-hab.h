
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <pthread.h>
#include <glib.h>
#include <bp.h>         /* Needs ion-dev */

#include "hardware-abstractor.h"

// usage.c
extern void usage();

// handle-hab-read.c
extern int bionet_fd;
extern int require_security;
extern char * security_dir;
extern char *hab_type;
extern char *hab_id;
extern bionet_hab_t *hab;
extern pthread_t handle_hab_read_thread;
extern void *handle_hab_read(void *);

// handle-bundles.c
extern Sdr sdr;
extern BpSAP sap;
extern char *eid;
extern pthread_t handle_bundles_thread;
extern void *handle_bundles(void *);

// bpstats2-hab.c
extern int keep_running;
extern pthread_mutex_t  doing_bionet_mutex;
