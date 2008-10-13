
//
// common header file for mDNS-SD/BIP CAL Client & Server code
//


#ifndef __CAL_MDNSSD_BIP_H
#define __CAL_MDNSSD_BIP_H


#include "cal-util.h"




#define Max(a, b) ((a) > (b) ? (a) : (b))

#define BIP_MSG_BUFFER_SIZE (1024)

#define CAL_MDNSSD_BIP_SERVICE_NAME "_bionet._tcp"


#define BIP_MSG_HEADER_SIZE (5)

#define BIP_MSG_HEADER_TYPE_OFFSET (0)
#define BIP_MSG_HEADER_SIZE_OFFSET (1)

#define BIP_MSG_TYPE_MESSAGE   (0)
#define BIP_MSG_TYPE_SUBSCRIBE (1)
#define BIP_MSG_TYPE_PUBLISH   (2)




// 
// helper functions used by both the Client and Server *threads*.
//

int bip_sendto(const cal_peer_t *peer, const void *msg, int size);


//! 
//! \brief Reads data from a peer, up to one complete packet.
//!
//! \param peer The peer to read from.  The data will be read into the end
//!     of the peer's buffer.
//!
//! \return -1 on failure (in which case the peer needs to be disconnected
//!     by the caller).  0 indicates some data was read but a complete
//!     packet is not available yet.  1 indicates exactly one packet is in
//!     the peer's buffer.
//!

int bip_read_from_peer(cal_peer_t *peer);




#endif  // __CAL_MDNSSD_BIP_H
