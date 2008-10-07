
#ifndef __CAL_SERVER_MDNSSD_BIP_H
#define __CAL_SERVER_MDNSSD_BIP_H


#include <pthread.h>

#include <dns_sd.h>
#include <glib.h>

#include "cal-util.h"

#include "cal-mdnssd-bip.h"




#define ID "CAL Server mDNS-SD/BIP "


extern void (*cal_server_mdnssd_bip_callback)(cal_event_t *event);

// pipes between CAL Server thread and user thread
extern int cal_server_mdnssd_bip_fds_to_user[2];
extern int cal_server_mdnssd_bip_fds_from_user[2];

extern int cal_server_mdnssd_bip_listening_socket;


extern pthread_t *cal_server_mdnssd_bip_thread;
void *cal_server_mdnssd_bip_function(void *peer_as_voidp);


extern DNSServiceRef *cal_server_mdnssd_bip_advertisedRef;




#endif  //  __CAL_SERVER_MDNSSD_BIP_H

