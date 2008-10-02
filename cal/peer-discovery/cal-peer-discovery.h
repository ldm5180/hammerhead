
#ifndef __CAL_PEER_DISCOVERY_H
#define __CAL_PEER_DISCOVERY_H


#include "cal-peer.h"
#include "cal-event.h"




typedef struct {
    //
    // these make a peer "findable" by others:
    //

    // Inform the network that the peer "me" has joined
    // returns TRUE (non-zero) on success, FALSE (zero) on failure
    int (*join)(cal_peer_t *me);

    // Inform the network that the peer "me" has left
    // returns TRUE (non-zero) on success, FALSE (zero) on failure
    int (*leave)(cal_peer_t *me);


    // 
    // these let a peer see what other peers are out there:
    //

    // starts tracking the peer list
    // creates a file descriptor for the user to monitor, the user must call cal_pd->read() when the fd is readable
    // returns the fd, or -1 on error
    int (*subscribe_peer_list)(void (*callback)(cal_event_t *event));

    // user should call this function when the fd returned from subscribe() is readable
    // this function may call the callback registered with the subscribe() call
    // returns TRUE (non-zero) on success, FALSE (0) on failure
    int (*read)(void);

    // stops tracking the peer list
    // the fd from subscribe_peer_list() is invalid after this function returns
    void (*unsubscribe_peer_list)(void);
} cal_pd_t;




#endif //  __CAL_PEER_DISCOVERY_H

