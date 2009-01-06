
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

#include <stdio.h>

#include "bionet-util.h"

#include "libhab-internal.h"
#include "hardware-abstractor.h"




int libhab_cal_fd = -1;

bionet_hab_t *libhab_this = NULL;

void (*libhab_callback_set_resource)(bionet_resource_t *resource, bionet_value_t *value) = NULL;
void (*libhab_callback_lost_client)(const char *client_id) = NULL;




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

