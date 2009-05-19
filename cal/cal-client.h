
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


//!
//! \file cal-client.h
//!
//! \brief the CAL Client interface
//!
//! \author Sebastian Kuzminsky <seb@highlab.com>
//!


#ifndef __CAL_CLIENT_H
#define __CAL_CLIENT_H

#include <sys/time.h>

#include "cal-event.h"




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
    //! \param network_type A NULL-terminated ASCII string containing the
    //!     name of the CAL network to join.
    //!
    //! \param callback The callback function to be called by the CAL
    //!     library when events happen to this peer.
    //!
    //! \param peer_matches This function is called to evaluate whether a
    //!     peer matches the peer name of a subscription.  The function
    //!     should return 0 if the peer name matches, and non-zero if the
    //!     topic does not match.  Thus strcmp() is a valid option, and
    //!     results in only exact peer name matches being accepted.  This
    //!     is also the default if NULL is passed in for peer_matches.
    //!     peer_matches must be reenstrant.
    //!
    //! \return On success, returns a non-blocking file descriptor which
    //!     should be monitored by the caller.  When the fd is readable,
    //!     or if the caller wants to poll it, the caller should call the
    //!     .read() function.  The caller must never read or write the
    //!     returned file descriptor directly.
    //!     On failure, returns -1.
    //!

    int (*init)(
        const char *network_type,
        void (*callback)(const cal_event_t *event),
        int (*peer_matches)(const char *peer_name, const char *subscription)
    );


    //!
    //! \brief Leave the network.
    //!

    void (*shutdown)(void);


    //!
    //! \brief Subscribe to a new topic from a server.
    //!
    //! \param peer_name The name of the server to subscribe from.
    //!
    //! \param topic The topic to subscribe to.  A NULL-terminated ASCII
    //!     string.
    //!
    //! \return True (non-zero) on success, False (zero) on failure.
    //!

    int (*subscribe)(const char *peer_name, const char *topic);


    //!
    //! \brief Remove a subscription topic from a server.
    //!
    //! \param peer_name The name of the server to unsubscribe from.
    //!
    //! \param topic The topic to remove.  A NULL-terminated ASCII
    //!     string.
    //!
    //! \return True (non-zero) on success, False (zero) on failure.
    //!

    int (*unsubscribe)(const char *peer_name, const char *topic);


    //!
    //! \brief Service the CAL-Client file descriptor.
    //!
    //! This function should be called whenever the file descriptor
    //! returned from .init() is readable, or whenever the caller wants to
    //! poll the file descriptor.  This function will service the file
    //! descriptor and may call the callback with an appropriate event.
    //!
    //! \return True (non-zero) on success.  False (zero) on failure,
    //!     indicating that the file descriptor returned from .init() is
    //!     invalid and the caller needs to call .shutdown() followed by 
    //!     .init() to get a new fd.
    //!

    int (*read)(struct timeval *timeout);


    //!
    //! \brief Send a message to a server.
    //!
    //! This function sends a message (just an array of bytes) to a server.
    //!
    //! \param peer The name of the server to send the message to.
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

    int (*sendto)(const char *peer_name, void *msg, int size);


    /**
     * @brief Initialize security parameters.
     *
     * @param[in] dir Directory containing security certificates 
     * and configuration
     * @param[in] require 0 for optional security, 1 for required 
     * security. (default: 0)
     *
     * @return 1 Succes
     * @return 0 Failure
     */
    int (*init_security)(const char * dir, int require);

} cal_client_t;


//!< the CAL Client module provides this
extern cal_client_t cal_client;




#endif //  __CAL_CLIENT_H

