
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>

#include "bionet-util.h"

#include "libhab-internal.h"
#include "hardware-abstractor.h"




void * libhab_cal_handle = NULL;

bionet_hab_t *libhab_this = NULL;

void (*libhab_callback_set_resource)(bionet_resource_t *resource, bionet_value_t *value) = NULL;
void (*libhab_callback_stream_data)(const char *client_id, bionet_stream_t *stream, const void *buf, unsigned int size) = NULL;
void (*libhab_callback_lost_client)(const char *client_id) = NULL;
void (*libhab_callback_stream_subscription)(const char *client_id, const bionet_stream_t *stream) = NULL;
void (*libhab_callback_stream_unsubscription)(const char *client_id, const bionet_stream_t *stream) = NULL;

char * persist_dir = "/var/lib/bionet";


#if 0

GSList *libhab_queued_messages_from_nag = NULL;


char *libhab_nag_hostname = NULL;
unsigned short libhab_nag_port = 11001;

int libhab_nag_timeout = 5;

char *libhab_type = NULL;
char *libhab_id = NULL;


bionet_nxio_t *libhab_nag_nxio = NULL;


char *libhab_nag_error = NULL;

#endif

