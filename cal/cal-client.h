
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

// This library is free software. You can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as 
// published by the Free Software Foundation, version 2.1 of the License.
// This library is distributed in the hope that it will be useful, but 
// WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details. A copy of the GNU 
// Lesser General Public License v 2.1 can be found in the file named 
// "COPYING.LESSER".  You should have received a copy of the GNU Lesser 
// General Public License along with this library; if not, write to the 
// Free Software Foundation, Inc., 
// 51 Franklin Street, Fifth Floor, 
// Boston, MA 02110-1301 USA.
 
// You may contact the Automation Group at:
// bionet@bioserve.colorado.edu
 
// Dr. Kevin Gifford
// University of Colorado
// Engineering Center, ECAE 1B08
// Boulder, CO 80309
 
// Because BioNet was developed at a university, we ask that you provide
// attribution to the BioNet authors in any redistribution, modification, 
// work, or article based on this library.
 
// You may contribute modifications or suggestions to the University of
// Colorado for the purpose of discussing and improving this software.
// Before your modifications are incorporated into the master version 
// distributed by the University of Colorado, we must have a contributor
// license agreement on file from each contributor. If you wish to supply
// the University with your modifications, please join our mailing list.
// Instructions can be found on our website at 
// http://bioserve.colorado.edu/developers-corner.

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

#if defined(__WIN32)
#  if defined(BIONET_VERSION)
#    ifdef DLL_EXPORT
#      define API_DECL __declspec(dllexport)
#    else
#      define API_DECL
#    endif
#  else
#    define API_DECL __declspec(dllimport)
#  endif
#else
#  define API_DECL extern
#endif



typedef struct {

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
    //!     peer_matches must be reentrant.
    //!
    //! \return On success, returns an opaque pointer to a CAL Client
    //!     context.  You can call the .get_fd() function to get a non-
    //!     blocking file descriptor from this context which should be
    //!     monitored by the caller.  When the fd is readable, or if the
    //!     caller wants to poll it, the caller should call the .read()
    //!     function.  The caller must never read or write the returned
    //!     file descriptor directly.
    //!     On failure, returns NULL.
    //!

    void * (*init)(
        const char *network_type,
        void (*callback)(void * cal_handle, const cal_event_t *event),
        int (*peer_matches)(const char *peer_name, const char *subscription),
	void * ssl_ctx,
	int require_security
    );


    //!
    //! \brief Leave the network.
    //! \param[in] cal_handle Pointer to CAL context
    //!

    void (*shutdown)(void * cal_handle);


    //!
    //! \brief Subscribe to a new topic from a server.
    //!
    //! \param[in] cal_handle Pointer to CAL context
    //! \param peer_name The name of the server to subscribe from.
    //!
    //! \param topic The topic to subscribe to.  A NULL-terminated ASCII
    //!     string.
    //!
    //! \return True (non-zero) on success, False (zero) on failure.
    //!

    int (*subscribe)(void * cal_handle, const char *peer_name, const char *topic);


    //!
    //! \brief Remove a subscription topic from a server.
    //!
    //! \param[in] cal_handle Pointer to CAL context
    //! \param peer_name The name of the server to unsubscribe from.
    //!
    //! \param topic The topic to remove.  A NULL-terminated ASCII
    //!     string.
    //!
    //! \return True (non-zero) on success, False (zero) on failure.
    //!

    int (*unsubscribe)(void * cal_handle, const char *peer_name, const char *topic);


    //!
    //! \brief Service the CAL-Client file descriptor.
    //!
    //! This function should be called whenever the file descriptor
    //! returned from .init() is readable, or whenever the caller wants to
    //! poll the file descriptor.  This function will service the file
    //! descriptor and may call the callback with an appropriate event.
    //! 
    //! \param max_events 
    //!   At most how many events to read before returning
    //!
    //! \return True (non-zero) on success.  False (zero) on failure,
    //!     indicating that the file descriptor returned from .init() is
    //!     invalid and the caller needs to call .shutdown() followed by 
    //!     .init() to get a new fd.
    //!

    int (*read)(void * cal_handle, struct timeval *timeout, unsigned int max_events);


    //!
    //! \brief Send a message to a server.
    //!
    //! This function sends a message (just an array of bytes) to a server.
    //!
    //! \param[in] cal_handle Pointer to CAL context
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

    int (*sendto)(void * cal_handle, const char *peer_name, void *msg, int size);


    /**
     * @brief Initialize security parameters.
     *
     * @param[in] cal_handle Pointer to CAL context
     * @param[in] dir Directory containing security certificates 
     * and configuration
     * @param[in] require 0 for optional security, 1 for required 
     * security. (default: 0)
     *
     * @return NULL Failure
     * @return Otherwise Success
     */
    void * (*init_security)(const char * dir, int require);


    /**
     * @brief Get file descriptor
     *
     * @param[in] cal_handle Pointer to CAL context
     *
     * @return >=0 File descriptor
     * @return -1 Error
     */
    int (*get_fd)(void * cal_handle);
} cal_client_t;


//!< the CAL Client module provides this
API_DECL cal_client_t cal_client;




#endif //  __CAL_CLIENT_H

