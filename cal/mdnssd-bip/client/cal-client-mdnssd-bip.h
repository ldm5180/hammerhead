
#ifndef __CAL_MDNSSD_BIP_CLIENT_H
#define __CAL_MDNSSD_BIP_CLIENT_H


#include <pthread.h>

#include <dns_sd.h>
#include <glib.h>

#include "cal-mdnssd-bip.h"




#define ID "mDNS-SD/BIP Client "




typedef struct {
    int (*peer_matches)(const char *peer_name, const char *subscription);
} cal_client_mdnssd_bip_t;



extern char *cal_client_mdnssd_bip_network_type;

extern void (*cal_client_mdnssd_bip_callback)(cal_event_t *event);

// pipes between CAL Client thread and user thread
extern int cal_client_mdnssd_bip_fds_to_user[2];
extern int cal_client_mdnssd_bip_fds_from_user[2];




// the Client thread
extern pthread_t *cal_client_mdnssd_bip_thread;
void *cal_client_mdnssd_bip_function(void *arg);




#endif  //  __CAL_MDNSSD_BIP_CLIENT_H

