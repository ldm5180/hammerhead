
//!
//! \file cal-server.h
//!
//! \brief the CAL Server interface
//!
//! \author Sebastian Kuzminsky <seb@highlab.com>
//!


#ifndef __CAL_SERVER_H
#define __CAL_SERVER_H


#include "cal-util.h"




typedef struct {

    //!
    //! \brief A callback function provided by the user, to be called by
    //!     the CAL Server library whenever an event requires the user's
    //!     attention.
    //!
    //! Set by .init(), called by .read()
    //!
    //! The events are documented in the cal_event_t enum, in the
    //! cal-event.h file.
    //!
    //! \param event The event that requires the user's attention.  The
    //!     event is const, so the callback function should treat it as
    //!     read-only.
    //!

    void (*callback)(const cal_event_t *event);


    //!
    //! \brief Make this peer available for connections.
    //!
    //! \param this A handle for the CAL Peer that is going to start
    //!     accepting connections.  This peer should have the .name field
    //!     set to what the server's advertised name should be.  This
    //!     peer's addressing information will be set to indicate how other
    //!     peers can connect to it, and announced on the CAL network.
    //!
    //! \param callback The callback function to be called by the CAL
    //!     library when events happen to this peer.
    //!
    //! \return On success, returns a file descriptor which should be
    //!     monitored by the caller.  When the fd is readable, the caller
    //!     should call the .read() function.  The caller must never read
    //!     or write the returned file descriptor directly.  On failure,
    //!     returns -1.
    //!

    int (*init)(cal_peer_t *this, void (*callback)(const cal_event_t *event));


    //!
    //! \brief Withdraw from the network.
    //!

    void (*shutdown)(void);


    //!
    //! \brief Service the CAL file descriptor.
    //!
    //! This function should be called whenever the file descriptor
    //! returned from .init() is readable.  It will service the file
    //! descriptor and may call the callback with an appropriate event.
    //!
    //! \return True (non-zero) on success.  False (zero) on failure,
    //!     indicating that the file descriptor returned from
    //!     .init() is invalid and the caller needs to call .shutdown()
    //!     followed by .init() to get a new fd.
    //!

    int (*read)(void);


    //!
    //! \brief Send a message to a connected client.
    //!
    //! This function sends a message (just an array of bytes) to a
    //! connected client.
    //!
    //! \param peer The client to send the message to.  The peer pointer
    //!     must have been the subject of a previous Connect event, and not
    //!     the subject of a Disconnect event since then.
    //!
    //! \param msg The buffer to send.  The msg buffer must be dynamically
    //!     allocated, and it becomes the property of the CAL Server
    //!     library.  After passing the msg buffer to cal_server.sendto,
    //!     the user must not write to the buffer or free it.
    //!
    //! \param size The size of the buffer, in bytes.
    //!
    //! \returns True (non-zero) on success.  False (zero) on failure.
    //!

    int (*sendto)(cal_peer_t *peer, void *msg, int size);


    //!
    //! \brief Publish a message to all subscribed clients.
    //!
    //! \param topic The topic to publish on.  topic is a NULL-terminated
    //!     ASCII string.  When publish() returns, the caller may modify or
    //!     free the string.
    //!
    //! \param msg The content of the message to publish.  When publish()
    //!     returns, the caller may modify or free the msg buffer.
    //!
    //! \param size The size of msg, in bytes.
    //!

    void (*publish)(const char *topic, const void *msg, int size);

} cal_server_t;


//! the CAL Server module provides this
extern cal_server_t cal_server;




#endif //  __CAL_SERVER_H

