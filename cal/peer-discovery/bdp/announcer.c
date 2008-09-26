
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <arpa/inet.h>

#include <glib.h>

#include "cal.h"
#include "bdp.h"


// this is all the peers that have acked our Peer Info Replies lately
// we don't send any more Peer Info Replies to them
// keys are "IP:port" strings
// values are time_t's of when they acked us
// entries exire 1 minute after we get the ack
static GHashTable *peers_that_have_acked = NULL;




static int bdp_remove_peer_if_too_old(char *key__unused, time_t ack_time, time_t now) {
    if ((now - ack_time) > 60) {
        printf("peer '%s' is too old, removing from hash\n", key__unused);
        return TRUE;  // remove this peer
    }
    printf("peer '%s' is still young enough, keeping\n", key__unused);
    return FALSE;
}


static void bdp_expire_peers_that_have_acked(void) {
    time_t now;

    now = time(NULL);
    g_hash_table_foreach_remove(peers_that_have_acked, (GHRFunc)bdp_remove_peer_if_too_old, (gpointer)now);
}




// 
// The "announcer" thread runs this function.  It is started by bdp_join()
// and canceled by bdp_leave().
//
// Sends out heartbeats periodically, and responds to Peer Info Requests.
//

void *bdp_announcer_function(void *arg) {
    char heartbeat_packet[1024];  // FIXME: max packet size
    int heartbeat_packet_size;
    struct timeval prev_heartbeat;

    char peer_info_reply_packet[1024];  // FIXME: max packet size
    int peer_info_reply_packet_size;

    int r;

    cal_peer_t *peer = (cal_peer_t *)arg;


    //
    // the original Join message counts as a Heartbeat
    //

    r = gettimeofday(&prev_heartbeat, NULL);
    if (r < 0) {
        printf("bdp_announcer_thread: error with gettimeofday: %s\n", strerror(errno));
        return NULL;
    }


    // 
    // make the heartbeat packet
    //

    heartbeat_packet_size = snprintf(
        heartbeat_packet,
        sizeof(heartbeat_packet),
        "BDP Heartbeat\nPeer-Name: %s%c\n",
        peer->name,
        '\0'
    );
    if (heartbeat_packet_size >= sizeof(heartbeat_packet)) {
        printf("bdp_announcer_thread: error: bdp heartbeat packet too big!  %d bytes\n", heartbeat_packet_size);
        return NULL;
    }


    // 
    // make the Peer Info Reply packet
    //

    peer_info_reply_packet_size = snprintf(
        peer_info_reply_packet,
        sizeof(peer_info_reply_packet),
        "BDP Peer Info Reply\nPeer-Name: %s%c\n",
        peer->name,
        '\0'
    );
    if (peer_info_reply_packet_size >= sizeof(peer_info_reply_packet)) {
        printf("bdp_announcer_thread: error: bdp Peer Info Reply packet too big!  %d bytes\n", peer_info_reply_packet_size);
        return NULL;
    }


    // 
    // initialize the acked tables
    //

    peers_that_have_acked = g_hash_table_new_full(g_str_hash, g_str_equal, free, NULL);
    if (peers_that_have_acked == NULL) {
        printf("bdp_announcer_thread: error: cannot make peers_that_have_acked hash\n");
        return NULL;
    }


    while (1) {
        struct sockaddr_in from_addr;
        socklen_t from_len;

        ssize_t packet_size;
        uint8_t packet[1024];  // FIXME: max packet size

        struct timeval timeout;
        fd_set readers;
        int max_fd;

        struct timeval now;
        double t;


        // make sure the sockets are available
        if (bdp_mcast_socket == -1) {
            bdp_mcast_socket = bdp_make_mcast_socket(BDP_MCAST_ADDR, BDP_MCAST_PORT);
            if (bdp_mcast_socket < 0) {
                printf("bdp_announcer_thread: error making multicast socket\n");
                return NULL;
            }
        }
        if (bdp_unicast_socket == -1) {
            bdp_unicast_socket = bdp_make_unicast_socket();
            if (bdp_unicast_socket < 0) {
                printf("bdp_announcer_thread: error making unicast socket\n");
                return NULL;
            }
        }


        r = gettimeofday(&now, NULL);
        if (r < 0) {
            printf("bdp_announcer_thread: error with gettimeofday: %s\n", strerror(errno));
            return NULL;
        }
        t = bdp_tv_diff_double_seconds(&prev_heartbeat, &now);
        if (t > BDP_HEARTBEAT_PERIOD) {
            // send a heartbeat now, and set the timeout for next heartbeat timeout

            printf("bdp_announcer_thread: sending heartbeat packet\n");
            r = bdp_mcast_message(heartbeat_packet, heartbeat_packet_size);
            if (!r) {
                printf("bdp_announcer_thread: error sending heartbeat: %s\n", strerror(errno));
                return NULL;
            }

            prev_heartbeat = now;

            timeout.tv_sec = (int)BDP_HEARTBEAT_PERIOD;
            timeout.tv_usec = 1000 * 1000 * (BDP_HEARTBEAT_PERIOD - (int)BDP_HEARTBEAT_PERIOD);
        } else {
            double diff = (BDP_HEARTBEAT_PERIOD - t);
            timeout.tv_sec = (int)diff;
            timeout.tv_usec = 1000 * 1000 * (diff - (int)diff);
        }


        FD_ZERO(&readers);
        FD_SET(bdp_mcast_socket, &readers);
        FD_SET(bdp_unicast_socket, &readers);
        if (bdp_mcast_socket > bdp_unicast_socket) {
            max_fd = bdp_mcast_socket;
        } else {
            max_fd = bdp_unicast_socket;
        }

        printf("bdp_announcer_thread: entering select, timeout in %d.%06d seconds\n", (int)timeout.tv_sec, (int)timeout.tv_usec);
        r = select(max_fd + 1, &readers, NULL, NULL, &timeout);
        if (r < 0) {
            printf("bdp_announcer_thread: error in select: %s\n", strerror(errno));
            bdp_mcast_socket = -1;  // FIXME: cleanup?
            return NULL;
        }

        if (r == 0) {
            printf("bdp_announcer_thread: timeout\n");
            continue;
        }


        //
        // if we get here, then a peer sent a message to either our unicast or multicast socket
        // we try to read the unicast socket first
        //


        bdp_expire_peers_that_have_acked();


        from_len = sizeof(from_addr);

        if (FD_ISSET(bdp_unicast_socket, &readers)) {
            packet_size = recvfrom(bdp_unicast_socket, packet, sizeof(packet), 0, (struct sockaddr *)&from_addr, &from_len);
            if (packet_size == -1) {
                printf("bdp_announcer_thread: error in recvfrom: %s\n", strerror(errno));
                bdp_unicast_socket = -1;  // FIXME: cleanup?
                return NULL;
            }

            printf(
                "bdp_announcer_thread: received a %lu-byte unicast packet from %s:%d\n",
                (unsigned long)packet_size,
                inet_ntoa(from_addr.sin_addr),
                ntohs(from_addr.sin_port)
            );
        } else {
            // must be mcast
            packet_size = recvfrom(bdp_mcast_socket, packet, sizeof(packet), 0, (struct sockaddr *)&from_addr, &from_len);
            if (packet_size == -1) {
                printf("bdp_announcer_thread: error in recvfrom: %s\n", strerror(errno));
                bdp_mcast_socket = -1;  // FIXME: cleanup?
                return NULL;
            }

            printf(
                "bdp_announcer_thread: received a %lu-byte multicast packet from %s:%d\n",
                (unsigned long)packet_size,
                inet_ntoa(from_addr.sin_addr),
                ntohs(from_addr.sin_port)
            );
        }

        bdp_hexdump(packet, packet_size);

        if (
            (strncmp((char *)packet, "BDP Join", 8) == 0)
            || (strncmp((char *)packet, "BDP Heartbeat", 13) == 0)
            || (strncmp((char *)packet, "BDP Leave", 9) == 0)
            || (strncmp((char *)packet, "BDP Peer Info Reply", 19) == 0)
        ) {
            // the announcer ignores messages from other announcers
        } 

        else if (strncmp((char *)packet, "BDP Peer Info Request", 20) == 0) {
            char buffer[100];
            gpointer p;

            snprintf(buffer, sizeof(buffer), "%s:%d", inet_ntoa(from_addr.sin_addr), ntohs(from_addr.sin_port));
            p = g_hash_table_lookup(peers_that_have_acked, buffer);
            if (p == NULL) {
                printf("bdp_announcer_thread: sending Peer Info Reply\n");
                r = bdp_unicast_message(peer_info_reply_packet, peer_info_reply_packet_size, from_addr.sin_addr, ntohs(from_addr.sin_port));
                if (!r) {
                    printf("bdp_announcer_thread: error sending Peer Info Reply: %s\n", strerror(errno));
                    return NULL;
                }
            } else {
                printf("bdp_announcer_thread: %s has already Acked our Peer Info Request, ignoring\n", buffer);
            }
        } 

        else if (strncmp((char *)packet, "BDP Peer Info Ack", 17) == 0) {
            char buffer[100];
            char *key;
            gpointer p;
            time_t val;

            printf("bdp_announcer_thread: got a Peer Info Ack\n");

            snprintf(buffer, sizeof(buffer), "%s:%d", inet_ntoa(from_addr.sin_addr), ntohs(from_addr.sin_port));
            p = g_hash_table_lookup(peers_that_have_acked, buffer);
            if (p != NULL) {
                printf("bdp_announcer_thread: Peer Info Ack from %s is a duplicate, ignoring\n", buffer);
            } else {
                key = strdup(buffer);
                val = time(NULL);
                g_hash_table_insert(peers_that_have_acked, key, GINT_TO_POINTER(val));
                printf("bdp_announcer_thread: adding %s to peers_that_have_acked\n", buffer);
            }
        } 

        else {
            // unknown event?
            printf("bdp_announcer_thread: ignoring unknown packet!\n");
        }
    }
}

