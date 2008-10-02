
#ifndef __CAL_INFO_H
#define __CAL_INFO_H


#include "cal-peer.h"
#include "cal-event.h"




typedef struct {
    //
    // publisher stuff
    //

    // fills out this->addr and this->socket
    int (*init_publisher)(cal_peer_t *this, void (*callback)(cal_event_t *event));
    void (*publish)(char *topic, void *msg, int size);
    int (*publisher_read)(void);  // maybe calls callback


    //
    // subscriber stuff
    //

    int (*init_subscriber)(void (*callback)(cal_event_t *event));
    void (*cancel_subscriber)(void);
    void (*subscribe)(cal_peer_t *peer, char *topic);
    int (*subscriber_read)(void);  // maybe calls callback


    //
    // usable by both
    //

    int (*sendto)(cal_peer_t *peer, void *msg, int size);

} cal_i_t;




#endif //  __CAL_INFO_H

