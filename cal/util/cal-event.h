
///
/// \file cal-event.h
///
/// \brief the CAL Event interface
///
/// \author Sebastian Kuzminsky <seb@highlab.com>
///



#ifndef __CAL_EVENT_H
#define __CAL_EVENT_H




/// these are the CAL Event types
typedef enum {

    //! uninitialized or invalid event (the user should never see this)
    CAL_EVENT_NONE = 0,

    //! This event is delivered to clients who have called
    //! cal_client.init().  The event indicates that a server has joined
    //! the network.  The event->peer_name names the new server.
    //! The event->msg is empty and should be ignored.
    CAL_EVENT_JOIN,

    //! This event is delivered to clients who have called
    //! cal_client.init().  The event indicates that a previously Joined
    //! server has left the network.  The event->peer_name names the server
    //! that left, and corresponds to a peer_name that was previously the
    //! subject of a Join event.  No further communication with this server
    //! is possible, it's gone.  The event->msg is empty and should be
    //! ignored.
    CAL_EVENT_LEAVE,

    //! This event is delivered to servers who have called
    //! cal_server.init().  The event indicates that a client has
    //! connected to us.  The event->peer_name names the connected client.
    //! The event->msg is empty and should be ignored.
    CAL_EVENT_CONNECT,

    //! This event is delivered to servers who have called
    //! cal_server.init().  The event indicates that a previously
    //! connected client has disconnected from us.  The event->peer_name
    //! names the client that disconnected, and corresponds to a peer_name
    //! that was previously the subject of a Connect event.  No further
    //! communication with the client is possible, it's gone.
    //! The event->msg is empty and should be ignored.
    CAL_EVENT_DISCONNECT,

    //! This event indicates that a peer has sent us a message.  The
    //! event->peer_name names the peer that sent the message.  In a
    //! client, the peer is a server that the client has gotten a Join
    //! event from and has subsequently connected to.  In a server, the
    //! peer is a client that the server has gotten a Connect event from.
    //! The event->msg contains the message.  The event->msg.buffer is
    //! dynamically allocated by the CAL library.  This buffer will be
    //! freed when the user's callback function returns.  The user can
    //! steal the buffer by making a copy of the pointer and setting
    //! event->msg.buffer to NULL before returning, in which case it will
    //! not be freed by CAL library and it becomes the user's
    //! responsibility to free it when appropriate.
    CAL_EVENT_MESSAGE,

    //! This event means different things in the client and the server.
    //!
    //! In the client, when the user calls cal_client.subscribe() this
    //! event is sent to the CAL client thread, which records it and sends
    //! it to the appropriate server.
    //!
    //! In the server, the user thread gets this event from the CAL thread
    //! when a client has requested a new subscription.  The user thread
    //! decides whether to accept the subscription request or not.  If the
    //! user wants to accept the subscription, it sends a new event with
    //! the same peer name and topic back to the CAL thread (optionally
    //! stealing those fields from the original event).  If the user wants
    //! to drop (ignore) the subscription request, it simply does not send
    //! a subscribe event back to the CAL thread.
    CAL_EVENT_SUBSCRIBE,

    //! This event is delivered to CAL servers, to indicate that a CAL
    //! client has terminated an existing subscription.
    //!
    //! FIXME: in the awesome future, clients will be able to unsubscribe
    //! at will, currently the server synthesizes unsubscribe events to the
    //! server user thread when the client disconnects
    CAL_EVENT_UNSUBSCRIBE,

    //! This event is delivered to CAL clients, to indicate that a CAL
    //! server has published new information to us.  event->peer_name is
    //! the name of the server that published, and event->msg is the
    //! published message.  event->topic is unused and set to NULL.
    CAL_EVENT_PUBLISH,

    //! This event is used internally by the CAL server, and will never be
    //! seen by user threads.  It is used by the mDNS-SD/BIP BIP CAL server
    //! module, and in the future maybe other multi-threaded CAL modules.
    //!
    //! When the user thread calls the CAL init function, the init function
    //! waits to receive this event from the CAL thread before returning
    //! success to the caller.  All event_t fields other than event->type
    //! are unused set to 0.  If the CAL thread instead closes the pipe,
    //! the init function returns failure to the caller.
    CAL_EVENT_INIT,

    //! This event is used internally by the CAL server, and will never be
    //! seen by user threads.  It is used by the mDNS-SD/BIP BIP CAL server
    //! module, and in the future maybe other multi-threaded CAL modules.
    //!
    //! When the user thread calls the CAL shutdown function, the shutdown
    //! function sends this event to the CAL thread.  All event_t fields
    //! other than event->type are unused set to 0.  The CAL thread
    //! responds to this event by cleaning up (including closing its end of
    //! the pipes) and cancelling itself.
    CAL_EVENT_SHUTDOWN

} cal_event_type_t;


typedef struct {
    char *buffer;
    int size;
} cal_message_t;


/// this is the structure of a CAL Event
typedef struct {
    cal_event_type_t type;  ///< the event type

    char *peer_name;        //!< the name of the peer that the event concerns

    char *topic;            //!< for the Publish & Subscribe events, the subscription topic (NULL terminated ASCII string)
    cal_message_t msg;      ///< the message sent (only the CAL_EVENT_MESSAGE event type has this)
} cal_event_t;




///
/// \brief Allocates a new CAL Event of the given type.
///
/// \param type The type of the new event.
///
/// \return A dynamically allocated event with type set.  The caller is
///     responsible for freeing the event later.
///

cal_event_t *cal_event_new(cal_event_type_t type);


///
/// \brief Frees a CAL Event.
///
/// \param event The event to free.  The event's sub-fields (peer_name,
///     topic, & msg) will also be freed.
///

void cal_event_free(cal_event_t *event);




#endif //  __CAL_EVENT_H

