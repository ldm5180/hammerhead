
#ifndef __CAL_SERVER_MDNSSD_BIP_H
#define __CAL_SERVER_MDNSSD_BIP_H


#include <pthread.h>

#include <dns_sd.h>
#include <glib.h>

#include "cal-util.h"

#include "cal-mdnssd-bip.h"




#define ID "mDNS-SD/BIP "


// describes "this" server
typedef struct {
    char *name;
    uint16_t port;
    int socket;
    int (*topic_matches)(const char *a, const char *b);
} cal_server_mdnssd_bip_t;


extern void (*cal_server_mdnssd_bip_callback)(cal_event_t *event);

// pipes between CAL Server thread and user thread
extern int cal_server_mdnssd_bip_fds_to_user[2];
extern int cal_server_mdnssd_bip_fds_from_user[2];


extern pthread_t *cal_server_mdnssd_bip_thread;
void *cal_server_mdnssd_bip_function(void *this_as_voidp);




#endif  //  __CAL_SERVER_MDNSSD_BIP_H

