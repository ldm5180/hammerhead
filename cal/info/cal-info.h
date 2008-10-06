
//!
//! \file cal-info.h
//!
//! \brief the CAL Information interface
//!
//! \author Sebastian Kuzminsky <seb@highlab.com>
//!

#ifndef __CAL_INFO_H
#define __CAL_INFO_H


#include "cal-peer.h"
#include "cal-event.h"




//! This struct provides the CAL-I interface
typedef struct {
    //
    // publisher stuff
    //


    //! set by init_publisher(), called by publisher_read()
    void (*publisher_callback)(cal_event_t *event);


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
    //!     should call the CAL-I publisher_read() function.  The caller
    //!     must never read or write the returned file descriptor.
    //!     On failure, returns -1.
    //!

    int (*init_publisher)(cal_peer_t *this, void (*callback)(cal_event_t *event));


    //! \brief Just a place-holder for now
    void (*publish)(char *topic, void *msg, int size);


    //!
    //! \brief Service the CAL-I publisher's file descriptor.
    //!
    //! This function should be called whenever the file descriptor
    //! returned from init_publisher() is readable.  It will service the
    //! file descriptor and may call the callback with an appropriate
    //! event.
    //!
    //! \return True (non-zero) on success.  False (zero) on failure,
    //!     indicating that the file descriptor returned from
    //!     init_publisher() is invalid and the caller needs to re-call
    //!     init_publisher() to get a new fd.
    //!

    int (*publisher_read)(void);


    //!
    //! \brief Send a message to a connected client
    //!

    int (*publisher_sendto)(cal_peer_t *peer, void *msg, int size);




    //
    // subscriber stuff
    //


    //! set by init_subscriber(), called by subscriber_read()
    void (*subscriber_callback)(cal_event_t *event);


    int (*init_subscriber)(void (*callback)(cal_event_t *event));
    void (*cancel_subscriber)(void);
    void (*subscribe)(cal_peer_t *peer, char *topic);
    int (*subscriber_read)(void);  // maybe calls callback
    int (*subscriber_sendto)(cal_peer_t *peer, void *msg, int size);

} cal_i_t;




#endif //  __CAL_INFO_H

