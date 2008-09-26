
#ifndef __CAL_PD_DNSSD_H
#define __CAL_PD_DNSSD_H


#include <pthread.h>

#include "cal.h"


#define Max(a, b) ((a) > (b) ? (a) : (b))


extern const char *cal_pd_dnssd_service_name;

extern void (*cal_pd_dnssd_callback)(cal_event_t *event);


// HAL stuff
int dnssd_register(cal_peer_t* peer);
int dnssd_remove(cal_peer_t* peer);


//
// Client stuff
//

// the browser thread keeps track of what peers are out there
extern pthread_t *cal_pd_dnssd_browser_thread;
void *cal_pd_dnssd_browser_function(void *arg);

// this stops all pending service references
void cal_pd_dnssd_end_browse(void);

// pipe between browser thread and user code
extern int cal_pd_dnssd_browser_fds[2];




#endif  //  __CAL_PD_DNSSD_H

