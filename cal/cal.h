
// 
// This header file describes the Bionet 2 Communication Abstraction Layer.
//

#ifndef __CAL_H
#define __CAL_H


#include <sys/socket.h>




typedef struct {
    char *name;  // FIXME: freeform ascii string, no cryptographic protection or certification yet

    // this is the address that the peer is listening on, if known
    // if addr.sa_family == AF_UNSPEC (0), the peer address is unknown
    struct sockaddr addr; 

    // in a publisher, this is the listening socket; filled out by cal_i.init_publisher() and advertised by cal_pd.join()
    // in a subscriber, this has no meaning yet, but it will probably hold the subscriber's TCP socket to this publisher
    int socket;

    // it's up to the user to manage this one
    void *user_data;
} cal_peer_t;


cal_peer_t *cal_peer_new(const char *name);
void cal_peer_free(cal_peer_t *peer);




typedef enum {
    CAL_EVENT_NONE = 0,
    CAL_EVENT_JOIN,
    CAL_EVENT_LEAVE
} cal_event_type_t;


typedef struct {
    cal_event_type_t event_type;
    cal_peer_t *peer;
} cal_event_t;


cal_event_t *cal_event_new(cal_event_type_t type);
void cal_event_free(cal_event_t *event);




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




#endif //  __CAL_H

