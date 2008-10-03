
///
/// \file cal-event.h
///
/// \brief the CAL Event interface
///
/// \author Sebastian Kuzminsky <seb@highlab.com>
///



#ifndef __CAL_EVENT_H
#define __CAL_EVENT_H


#include "cal-peer.h"




/// these are the CAL Event types
typedef enum {
    CAL_EVENT_NONE = 0,   ///< uninitialized event
    CAL_EVENT_JOIN,       ///< we've noticed that a peer has joined the network
    CAL_EVENT_LEAVE,      ///< we've noticed that a peer has left the network
    CAL_EVENT_CONNECT,    ///< a peer has connected to us
    CAL_EVENT_DISCONNECT, ///< a peer has disconnected from us
    CAL_EVENT_MESSAGE     ///< a peer has sent us a message
} cal_event_type_t;


typedef struct {
    char *buffer;
    int size;
} cal_message_t;


/// this is the structure of a CAL Event
typedef struct {
    cal_event_type_t type;  ///< the event type
    cal_peer_t *peer;       ///< the peer that generated the event (all events have this)
    cal_message_t msg;      ///< the message sent (only the CAL_EVENT_MESSAGE event type has this)
} cal_event_t;




///
/// \brief Allocates a new CAL Event of the given type.
///
/// \param type The type of the new event.
///
/// \return A dynamically allocated event with type set.  event->peer will
///     be initalized to NULL.  The caller is responsible for freeing the
///     event later.
///

cal_event_t *cal_event_new(cal_event_type_t type);


///
/// \brief Frees a CAL Event.
///
/// \param event The event to free.  The event's peer will also be freed.
///

void cal_event_free(cal_event_t *event);




#endif //  __CAL_EVENT_H

