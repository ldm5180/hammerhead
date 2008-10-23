
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

    //! This event is delivered to clients who have called
    //! cal_client.init().  The event indicates that a server has joined
    //! the network.  The event->peer describes the new server.  The client
    //! should not modify the peer!  The peer pointer will remain valid
    //! until the matching Leave event happens.  The event->msg is empty
    //! and should be ignored.
    CAL_EVENT_JOIN,

    //! This event is delivered to clients who have called
    //! cal_client.init().  The event indicates that a previously Joined
    //! server has left the network.  The event->peer describes the server
    //! that left, and points to a cal_peer_t that was previously the
    //! subject of a Join event.  No further communication with this server
    //! is possible, it's gone.  The peer pointer will remain valid until
    //! the user's callback returns, then it will be made undefined.  The
    //! event->msg is empty and should be ignored.
    CAL_EVENT_LEAVE,

    //! This event is delivered to servers who have called
    //! cal_server.init().  The event indicates that a client has
    //! connected to us.  The event->peer describes the connected client.
    //! The server should not modify the peer!  The peer pointer will
    //! remain valid until the matching Disconnect event happens.  The
    //! event->msg is empty and should be ignored.
    CAL_EVENT_CONNECT,

    //! This event is delivered to servers who have called
    //! cal_server.init().  The event indicates that a previously
    //! connected client has disconnected from us.  The event->peer
    //! describes the client that disconnected, and points to a cal_peer_t
    //! that was previously the subject of a Connect event.  No further
    //! communication with the client is possible, it's gone.  The peer
    //! pointer will remain valid until the user's callback returns, then
    //! it will be made undefined.  The event->msg is empty and should be
    //! ignored.
    CAL_EVENT_DISCONNECT,

    //! This event indicates that a peer has sent us a message.  The
    //! event->peer describes the peer that sent the message.  In a client,
    //! the peer is a server that the client has gotten a Join event from
    //! and has subsequently connected to.  In a server, the peer is a
    //! client that the server has gotten a Connect event from.  The
    //! event->msg contains the message.  The event->msg.buffer is
    //! dynamically allocated by the CAL library.  This buffer will be
    //! freed when the user's callback function returns.  The user can
    //! steal the buffer by making a copy of the pointer and setting
    //! event->msg.buffer to NULL before returning, in which case it will
    //! not be freed by CAL library and it becomes the user's
    //! responsibility to free it when appropriate.
    //! FIXME: how to manage event->peer in subscribers?  give them Connect
    //! and Disconnect events?
    CAL_EVENT_MESSAGE,

    //! This event indicates that a client has registered a new
    //! subscription with us.
    CAL_EVENT_SUBSCRIBE,

    //! This event indicates that a server has published new information to
    //! us.
    CAL_EVENT_PUBLISH

} cal_event_type_t;


typedef struct {
    char *buffer;
    int size;
} cal_message_t;


/// this is the structure of a CAL Event
typedef struct {
    cal_event_type_t type;  ///< the event type

    cal_peer_t *peer;       //!< the peer that the event concerns (all events have this or peer_name)
    char *peer_name;        //!< the name of the peer that the event concerns (all events have this or peer)

    char *topic;            //!< for the Publish & Subscribe events, the subscription topic (NULL terminated ASCII string)
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

