
// Copyright (c) 2008-2009, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "pal-650-hab.h"


bionet_hab_t *hab = NULL;

char *pal_ip = NULL;
int pal_port = 5117;
int tag_timeout = 5;
int show_messages = 0;

char * security_dir = NULL;
