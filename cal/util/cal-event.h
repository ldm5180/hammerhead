
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

    //! uninitialized or invalid event (the user should never see this)
    CAL_EVENT_NONE = 0,

    //! This event is delivered to peers who have called
    //! cal_pd.subscriber_peer_list().  The event indicates that a peer has
    //! joined the network.  The event->peer describes the new peer.  The
    //! peer pointer will remain valid until the matching Leave event
    //! happens.  The event->msg is empty and should be ignored.
    CAL_EVENT_JOIN,

    //! This event is delivered to peers who have called
    //! cal_pd.subscriber_peer_list().  The event indicates that a
    //! previously Joined peer has left the network.  No further
    //! communication with this peer is possible, it's gone.  The
    //! event->peer describes the peer that left.  The peer pointer will
    //! remain valid until the user's callback returns, then it will be
    //! made undefined.  The event->msg is empty and should be ignored.
    CAL_EVENT_LEAVE,

    //! This event is delivered to peers who have called
    //! cal_i.init_publisher().  The event indicates that a peer has
    //! connected to us.  The event->peer describes the connected peer.
    //! The peer pointer will remain valid until the matching Disconnect
    //! event happens.  The event->msg is empty and should be ignored.
    CAL_EVENT_CONNECT,

    //! This event is delivered to peers who have called
    //! cal_i.init_publisher().  The event indicates that a previously
    //! connected peer has disconnected from us.  The event->peer describes
    //! the peer that disconnected.  No further communication with the peer
    //! is possible, it's gone.  The peer pointer will remain valid until
    //! the user's callback returns, then it will be made undefined.
    //! The event->msg is empty and should be ignored.
    CAL_EVENT_DISCONNECT,

    //! This event indicates that a peer has sent us a message.  The
    //! event->peer describes the peer that sent the message, and
    //! event->msg contains the message.  The event->msg.buffer is
    //! dynamically allocated by the CAL-I library, and becomes the
    //! property of the user's callback function.  The user's callback must
    //! free it or otherwise manage the memory to avoid memory leaks here.
    //! FIXME: how to manage event->peer in subscribers?  give them Connect
    //! and Disconnect events?
    CAL_EVENT_MESSAGE

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

