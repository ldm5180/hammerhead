
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "config.h"

#include "cal-util.h"


#ifdef _WIN32
typedef struct {
    int user; //inet socket
    int thread; //inet socket
} bip_msg_queue_t; 

#define bip_msg_queue_get_handle(q,dir) (dir==BIP_MSG_QUEUE_TO_USER?(q)->user:(q)->thread)

#else
typedef struct {
    int to_user[2]; // pipe(2)
    int from_user[2]; // pipe(2)
} bip_msg_queue_t; 
#define bip_msg_queue_get_handle(q,dir) (dir==BIP_MSG_QUEUE_TO_USER?(q)->to_user[0]:(q)->from_user[0])

#endif

typedef enum {
    BIP_MSG_QUEUE_TO_USER = 0,
    BIP_MSG_QUEUE_FROM_USER
} bip_msg_queue_direction_t;

/**
 * Init the queue, with a reference count of 1
 */
int bip_msg_queue_init(bip_msg_queue_t *q);

/**
 * Add an additional reference to the queue
 */
void bip_msg_queue_ref(bip_msg_queue_t *q);

/**
 * Remove a reverence to the queue.
 *
 * If this is the last reference, free all memory
 */
void bip_msg_queue_unref(bip_msg_queue_t *q);

/**
 * Send a close signal to the reciever.
 */
int bip_msg_queue_close(bip_msg_queue_t *q, bip_msg_queue_direction_t dir);


/**
 * Push an event onto the queue.
 *
 * return < 0 on error
 */
int bip_msg_queue_push(
        bip_msg_queue_t *q,
        bip_msg_queue_direction_t dir,
        cal_event_t * event);

/**
 * Pop an event off the queue
 *
 * @return 0 on Success
 * @return < 0 on error
 * @return 1 when the queue has been closed by the sender
 */
int bip_msg_queue_pop(
        bip_msg_queue_t *q,
        bip_msg_queue_direction_t dir,
        cal_event_t ** event);


