
#ifndef __CAL_MDNSSD_BIP_CLIENT_H
#define __CAL_MDNSSD_BIP_CLIENT_H


#include <pthread.h>

#include <dns_sd.h>
#include <glib.h>

#include "cal-util.h"

#include "cal-mdnssd-bip.h"




#define ID "CAL Client mDNS-SD/BIP "


extern void (*cal_client_mdnssd_bip_callback)(cal_event_t *event);

// pipes between CAL Client thread and user thread
extern int cal_client_mdnssd_bip_fds_to_user[2];
extern int cal_client_mdnssd_bip_fds_from_user[2];




// the Client thread
extern pthread_t *cal_client_mdnssd_bip_thread;
void *cal_client_mdnssd_bip_function(void *arg);




//
// these are the CAL Client API functions
//

int cal_client_mdnssd_bip_init(void (*callback)(cal_event_t *event));
void cal_client_mdnssd_bip_shutdown(void);

void cal_client_mdnssd_bip_subscribe(cal_peer_t *peer, char *topic);

int cal_client_mdnssd_bip_read(void);

int cal_client_mdnssd_bip_sendto(cal_peer_t *peer, void *msg, int size);




#endif  //  __CAL_MDNSSD_BIP_CLIENT_H

