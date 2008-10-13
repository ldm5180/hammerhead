
//!
//! \file cal-client.h
//!
//! \brief the CAL Client interface
//!
//! \author Sebastian Kuzminsky <seb@highlab.com>
//!


#ifndef __CAL_CLIENT_H
#define __CAL_CLIENT_H


#include "cal-util.h"




typedef struct {

    //!
    //! \brief A callback function provided by the user, to be called by
    //!     the CAL Client library whenever an event requires the user's
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
    //! \brief Join the network and start looking for servers.
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

    int (*init)(void (*callback)(const cal_event_t *event));


    void (*shutdown)(void);


    void (*subscribe)(cal_peer_t *peer, char *topic);


    //!
    //! \brief Service the CAL-Client file descriptor.
    //!
    //! This function should be called whenever the file descriptor
    //! returned from .init() is readable.  It will service the file
    //! descriptor and may call the callback with an appropriate event.
    //!
    //! \return True (non-zero) on success.  False (zero) on failure,
    //!     indicating that the file descriptor returned from .init() is
    //!     invalid and the caller needs to call .shutdown() followed by 
    //!     .init() to get a new fd.
    //!

    int (*read)(void);


    //!
    //! \brief Send a message to a server.
    //!
    //! This function sends a message (just an array of bytes) to a server.
    //!
    //! \param peer The server to send the message to.  The peer pointer
    //!     must have been the subject of a previous Join event, and not
    //!     the subject of a Leave event since then.
    //!
    //! \param msg The buffer to send.  The msg buffer must be dynamically
    //!     allocated, and it becomes the property of the CAL Client
    //!     library.  After passing the msg buffer to cal_client.sendto,
    //!     the user must not write to the buffer or free it.
    //!
    //! \param size The size of the buffer, in bytes.
    //!
    //! \returns True (non-zero) on success.  False (zero) on failure.
    //!

    int (*sendto)(cal_peer_t *peer, void *msg, int size);

} cal_client_t;


//!< the CAL Client module provides this
extern cal_client_t cal_client;




#endif //  __CAL_CLIENT_H

