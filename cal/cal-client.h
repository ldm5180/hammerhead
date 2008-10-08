
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

    //! set by .init(), called by .read()
    void (*callback)(cal_event_t *event);


    // starts tracking the peer list
    // creates a file descriptor for the user to monitor, the user must call .read() when the fd is readable
    // returns the fd, or -1 on error
    int (*init)(void (*callback)(cal_event_t *event));


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


extern cal_client_t cal_client;  //!< the CAL Client library provides this




#endif //  __CAL_CLIENT_H

