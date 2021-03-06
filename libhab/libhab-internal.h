
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef __LIBHAB_INTERNAL_H
#define __LIBHAB_INTERNAL_H




#include <glib.h>

#include "cal-server.h"
#include "bionet-util.h"




extern void * libhab_cal_handle;

extern bionet_hab_t *libhab_this;

extern GHashTable * libhab_most_recently_published;

extern void * libhab_ssl_ctx;
extern int libhab_require_security;

extern char * persist_dir;

int libhab_cal_topic_matches(const char *topic, const char *subscription);

void libhab_cal_callback(void * cal_handle, const cal_event_t *event);

void libhab_datapoint_destructor(bionet_datapoint_t * dp, void * hash_table);

extern void (*libhab_callback_set_resource)(bionet_resource_t *resource, bionet_value_t *value);
extern void (*libhab_callback_stream_data)(const char *client_id, bionet_stream_t *stream, const void *buf, unsigned int size);
extern void (*libhab_callback_stream_subscription)(const char *client_id, const bionet_stream_t *stream);
extern void (*libhab_callback_stream_unsubscription)(const char *client_id, const bionet_stream_t *stream);
extern void (*libhab_callback_lost_client)(const char *client_id);



#endif // __LIBHAB_INTERNAL_H


