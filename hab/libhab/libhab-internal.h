
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef __LIBHAB_INTERNAL_H
#define __LIBHAB_INTERNAL_H




#include <glib.h>

#include "cal-server.h"
#include "bionet-util.h"




extern int libhab_cal_fd;

extern bionet_hab_t *libhab_this;




int libhab_cal_topic_matches(const char *topic, const char *subscription);

void libhab_cal_callback(const cal_event_t *event);




#if 0

extern GSList *libhab_queued_messages_from_nag;

extern int libhab_nag_timeout;

extern char *libhab_type;
extern char *libhab_id;

extern char *libhab_nag_error;




extern void (*libhab_callback_set_resource)(const char *node_id, const char *resource_id, const char *value);




int libhab_send_to_nag(const char *fmt, ...);

int libhab_send_to_nag_partial(const char *fmt, ...);
int libhab_send_to_nag_done(void);

bionet_message_t *libhab_read_from_nag(void);
int libhab_read_ok_from_nag(void);

void libhab_queue_nag_message(bionet_message_t *message);

#endif




#endif // __LIBHAB_INTERNAL_H


