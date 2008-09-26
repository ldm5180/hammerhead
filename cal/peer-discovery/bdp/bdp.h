
#ifndef  __BDP_H
#define  __BDP_H


#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <netinet/in.h>

#include "cal.h"


#define BDP_MCAST_ADDR "239.193.84.50"
#define BDP_MCAST_PORT 9876

#define BDP_HEARTBEAT_PERIOD   (2.0)
#define BDP_HEARTBEAT_TIMEOUT  (3 * BDP_HEARTBEAT_PERIOD)

#define BDP_SECONDS_BETWEEN_PEER_INFO_REQUESTS  (2.0)


// the 'browser' personality of the bdp module attaches one of these to each peer
typedef struct {
    time_t last_seen;

    // this is the peer's BDP unicast address
    struct in_addr addr;
    uint16_t port;
} bdp_peer_user_data_t;


extern int bdp_mcast_socket;     // UDP socket for receiving multicast packets to the BDP multicast group
extern int bdp_unicast_socket;   // BDP Peers send all packets from this socket, and receive unicast packets


// the "announcer" thread handles joining and leaving
// the "browser" thread keeps track of what peers are out there
extern pthread_t *bdp_announcer_thread;
extern pthread_t *bdp_browser_thread;


extern int bdp_pipe_fds[2];




//
// misc util functions
//


void bdp_generate_join_event(cal_peer_t *peer);
void bdp_generate_leave_event(cal_peer_t *peer);


void bdp_send_peer_info_ack(cal_peer_t *peer);


// function: make a socket, enable multicast, join the specified group, bind to the specified port
// returns the socket on success, -1 on failure
int bdp_make_mcast_socket(const char *mcast_addr, uint16_t mcast_port);

// returns the socket on success, -1 on failure
int bdp_make_unicast_socket(void);


int bdp_mcast_message(void *buffer, int size);
int bdp_unicast_message(void *buffer, int size, struct in_addr dest_addr, int dest_port);


void bdp_hexdump(uint8_t *packet, int packet_size);

double bdp_tv_diff_double_seconds(const struct timeval *start, const struct timeval *stop);


//
// the announcer and browser threads run these functions
//

void *bdp_announcer_function(void *arg);
void *bdp_browser_function(void *arg);


#endif  //  __BDP_H

