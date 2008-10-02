
#ifndef __CAL_EVENT_H
#define __CAL_EVENT_H


#include <stdint.h>

#include <sys/socket.h>

#include "cal-peer.h"




typedef enum {
    CAL_EVENT_NONE = 0,
    CAL_EVENT_JOIN,       // we've noticed that a peer has joined the network
    CAL_EVENT_LEAVE,      // we've noticed that a peer has left the network
    CAL_EVENT_CONNECT     // a peer has connected to us
} cal_event_type_t;


typedef struct {
    cal_event_type_t type;
    cal_peer_t *peer;
} cal_event_t;


cal_event_t *cal_event_new(cal_event_type_t type);
void cal_event_free(cal_event_t *event);




#endif //  __CAL_EVENT_H

