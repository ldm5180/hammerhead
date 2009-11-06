
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


//!
//! \file cal-server.h
//!
//! \brief the CAL Server interface
//!
//! \author Sebastian Kuzminsky <seb@highlab.com>
//!


#ifndef __CAL_SERVER_H
#define __CAL_SERVER_H

#include <sys/time.h>

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
    //! \param network_type A NULL-terminated ASCII string containing the
    //!     name of the CAL network to join.
    //!
    //! \param name The name to announce this CAL server as on the network.
    //!
    //! \param callback The callback function to be called by the CAL
    //!     library when events happen to this peer.
    //!
    //! \param topic_matches The topic_matches function will be called by
    //!     the CAL Server library to check if the topic of a published
    //!     message matches a CAL Client's subscription.  The function
    //!     should return 0 if the topic matches, and non-zero if the topic
    //!     does not match.  Thus strcmp() is a valid option, and results
    //!     in only exact topic matches being accepted.  This is also the
    //!     default if NULL is passed in for topic_matches.  topic_matches
    //!     must be reentrant.
    //!
    //! \return On success, returns a file descriptor which should be
    //!     monitored by the caller.  When the fd is readable, or when the
    //!     caller wants to poll it, the caller should call the .read()
    //!     function.  The caller must never read or write the returned
    //!     file descriptor directly.  On failure, returns -1.
    //!

    int (*init)(
        const char *network_name,
        const char *name,
        void (*callback)(const cal_event_t *event),
        int (*topic_matches)(const char *topic, const char *subscription)
    );


    //!
    //! \brief Withdraw from the network.
    //! 
    //! A "shutdown" event message is sent to the CAL Server thread, and
    //! the CAL Server thread handles this event by killing itself.
    //!
    //! All pending event messages from the CAL Server thread are handled
    //! by the user's registered callback function before this shutdown()
    //! function returns.
    //!

    void (*shutdown)(void);


    //!
    //! \brief Service the CAL file descriptor.
    //!
    //! This function should be called whenever the file descriptor
    //! returned from .init() is readable, or whenever the user wants to
    //! poll the file descriptor.  It will service the file descriptor and
    //! may call the callback with an appropriate event.
    //!
    //! \return True (non-zero) on success.  False (zero) on failure,
    //!     indicating that the file descriptor returned from
    //!     .init() is invalid and the caller needs to call .shutdown()
    //!     followed by .init() to get a new fd.
    //!

    int (*read)(struct timeval *timeout);


    //!
    //! \brief Add a subscription for a client.
    //!
    //! \param peer_name The name of the client that is subscribing.
    //!     The peer_name should have been the subject of a previous
    //!     Connect event, and not the subject of a Disconnect event since
    //!     then.
    //!
    //! \param topic The subscription topic to add.
    //!
    //! \returns True (non-zero) on success.  False (zero) on failure.
    //!

    int (*subscribe)(const char *peer_name, const char *topic);


    //!
    //! \brief Send a message to a connected client.
    //!
    //! This function sends a message (just an array of bytes) to a
    //! connected client.
    //!
    //! \param peer_name The name of the client to send the message to.
    //!     The peer_name should have been the subject of a previous
    //!     Connect event, and not the subject of a Disconnect event since
    //!     then.
    //!
    //! \param msg The buffer to send.  The msg buffer must be dynamically
    //!     allocated, and it becomes the property of the CAL Server
    //!     library (on success only).  After passing the msg buffer to cal_server.sendto,
    //!     the user must not write to the buffer or free it.  FIXME: Note
    //!     that this is opposite from how the .publish() msg behaves
    //!
    //! \param size The size of the buffer, in bytes.
    //!
    //! \returns True (non-zero) on success.  False (zero) on failure.
    //!

    int (*sendto)(const char *peer_name, void *msg, int size);


    //!
    //! \brief Publish a message to all subscribed clients.
    //!
    //! \param topic The topic to publish on.  topic is a NULL-terminated
    //!     ASCII string.  When publish() returns, the caller may modify or
    //!     free the string.
    //!
    //! \param msg The content of the message to publish.  When publish()
    //!     returns, the caller may modify or free the msg buffer.  FIXME:
    //!     Note that this is opposite from how the .sendto() msg behaves
    //!
    //! \param size The size of msg, in bytes.
    //!

    void (*publish)(const char *topic, const void *msg, int size);


    //!
    //! \brief Publish a message to a conected client
    //!
    //! \param peer_name The name of the client to send the message to.
    //!     The peer_name should have been the subject of a previous
    //!     Connect event, and not the subject of a Disconnect event since
    //!     then.
    //!
    //! \param topic The topic to publish on.  topic is a NULL-terminated
    //!     ASCII string.  When publish() returns, the caller may modify or
    //!     free the string.
    //!
    //! \param msg The content of the message to publish.  When publish()
    //!     returns, the caller may modify or free the msg buffer.  FIXME:
    //!     Note that this is opposite from how the .sendto() msg behaves
    //!
    //! \param size The size of msg, in bytes.
    //!

    void (*publishto)(const char *peer_name, const char *topic, const void *msg, int size);


    /**
     * @brief Initialize security parameters.
     *
     * @param[in] dir Directory containing security certificates 
     * and configuration
     * @param[in] require 0 for optional security, 1 for required 
     * security. (default: 0)
     *
     * @return 1 Success
     * @return 0 Failure
     */
    int (*init_security)(const char * dir, int require);

    //!
    //! \brief Make this peer available for connections.
    //!
    //! \param network_type A NULL-terminated ASCII string containing the
    //!     name of the CAL network to join.
    //!
    //! \param name The name to announce this CAL server as on the network.
    //!
    //! \param port The port to bind to, or -1 to pick a random available port
    //!
    //! \param callback The callback function to be called by the CAL
    //!     library when events happen to this peer.
    //!
    //! \param topic_matches The topic_matches function will be called by
    //!     the CAL Server library to check if the topic of a published
    //!     message matches a CAL Client's subscription.  The function
    //!     should return 0 if the topic matches, and non-zero if the topic
    //!     does not match.  Thus strcmp() is a valid option, and results
    //!     in only exact topic matches being accepted.  This is also the
    //!     default if NULL is passed in for topic_matches.  topic_matches
    //!     must be reentrant.
    //!
    //! \return On success, returns a file descriptor which should be
    //!     monitored by the caller.  When the fd is readable, or when the
    //!     caller wants to poll it, the caller should call the .read()
    //!     function.  The caller must never read or write the returned
    //!     file descriptor directly.  On failure, returns -1.
    //!

    int (*init_full)(
        const char *network_name,
        const char *name,
        int port,
        void (*callback)(const cal_event_t *event),
        int (*topic_matches)(const char *topic, const char *subscription)
    );


} cal_server_t;


//! the CAL Server module provides this
extern cal_server_t cal_server;




#endif //  __CAL_SERVER_H

