
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

    //! set by .init(), called by .read()
    void (*callback)(cal_event_t *event);


    //!
    //! \brief Make this peer available for connections.
    //!
    //! \param this A handle for the CAL Peer that is going to start
    //!     accepting connections.  This peer's addressing information will
    //!     be set to indicate how other peers can connect to it.
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

    int (*init)(cal_peer_t *this, void (*callback)(cal_event_t *event));


    //!
    //! \brief Withdraw this peer from the network.
    //!
    //! \param this The CAL Peer to withdraw.  Must be one that has
    //!     previously had .init() called on it.
    //!

    void (*shutdown)(cal_peer_t *this);


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
    //! \brief Send a message to a connected client
    //!

    int (*sendto)(cal_peer_t *peer, void *msg, int size);


    //! \brief Just a place-holder for now
    void (*publish)(char *topic, void *msg, int size);

} cal_server_t;


extern cal_server_t cal_server;




#endif //  __CAL_SERVER_H

