
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <glib.h>

#include "cal.h"
#include "bdp.h"


// the list of known peers, starts out empty
GHashTable *bdp_peers = NULL;




// 
// here comes the browser code
//
// There's a bdp_browser_function() and a couple of helper functions.
//

static void bdp_find_next_timeout(char *peer_name, cal_peer_t *peer, struct timeval *timeout) {
    bdp_peer_user_data_t *user_data = peer->user_data;
    time_t heartbeat_expiration;
    time_t time_until_expiration;

    printf("bdp_browser_thread: peer '%s' last seen at %d\n", peer_name, (int)user_data->last_seen);

    heartbeat_expiration = user_data->last_seen + BDP_HEARTBEAT_TIMEOUT;
    time_until_expiration = heartbeat_expiration - time(NULL);
    if (time_until_expiration < 0) {
        time_until_expiration = 0;
    }

    if (timeout->tv_sec > time_until_expiration) {
        timeout->tv_sec = time_until_expiration;
    }
}


static int bdp_remove_peer_if_expired(char *peer_name, cal_peer_t *peer, gpointer unused) {
    bdp_peer_user_data_t *user_data;
    time_t peer_expiration_time;

    user_data = peer->user_data;
    peer_expiration_time = user_data->last_seen + BDP_HEARTBEAT_TIMEOUT;
    if (time(NULL) < peer_expiration_time) {
        return FALSE; // don't remove this peer
    }

    free(user_data);
    peer->user_data = NULL;

    bdp_generate_leave_event(peer);

    return TRUE;
}


static void bdp_expire_peers(void) {
    g_hash_table_foreach_remove(bdp_peers, (GHRFunc)bdp_remove_peer_if_expired, NULL);
}




// 
// thread started by bdp_subscribe_peer_list(), canceled by bdp_unsubscribe_peer_list()
// listens for packets, turns them into events and writes them to the pipe
//

void *bdp_browser_function(void *arg) {
    int num_info_requests_sent = 0;


    printf("browser thread started\n");

    if (bdp_peers == NULL) {
        bdp_peers = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, (GDestroyNotify)cal_pd_free_peer);
    }

    while (1) {
        struct sockaddr_in from_addr;
        socklen_t from_len;
        int r;

        struct timeval timeout;
        fd_set readers;
        int max_fd;

        ssize_t packet_size;
        uint8_t packet[1024];  // FIXME: max packet size


        // make sure the sockets are available
        if (bdp_mcast_socket == -1) {
            bdp_mcast_socket = bdp_make_mcast_socket(BDP_MCAST_ADDR, BDP_MCAST_PORT);
            if (bdp_mcast_socket < 0) {
                printf("bdp_browser_thread: error making multicast socket\n");
                return 0;
            }
        }
        if (bdp_unicast_socket == -1) {
            bdp_unicast_socket = bdp_make_unicast_socket();
            if (bdp_unicast_socket < 0) {
                printf("bdp_browser_thread: error making unicast socket\n");
                return 0;
            }
        }


        // default timeout, if no one wants service sooner
        timeout.tv_sec = 9999999;
        timeout.tv_usec = 0;


        // send another Peer Info Request, if appropriate
        // FIXME: space these out
        if (num_info_requests_sent < 5) {
            static struct timeval prev = { 0, 0 };
            struct timeval now;
            double seconds_since_previous_packet;
            int r;

            r = gettimeofday(&now, NULL);
            if (r < 0) {
                printf("bdp_browser_thread: error with gettimeofday: %s\n", strerror(errno));
                return NULL;
            }

            seconds_since_previous_packet = bdp_tv_diff_double_seconds(&prev, &now);

            // at least two seconds between Peer Info Requests
            if (seconds_since_previous_packet >= BDP_SECONDS_BETWEEN_PEER_INFO_REQUESTS) {
                char packet[1024];  // FIXME: max packet size
                int packet_size;

                packet_size = snprintf(
                    packet,
                    sizeof(packet),
                    "BDP Peer Info Request\n"
                );
                if (packet_size >= sizeof(packet)) {
                    printf("bdp_browser_thread: error: bdp peer info request packet too big!  %d bytes\n", packet_size);
                    return 0;
                }

                printf("bdp_browser_thread: sending Peer Info Request\n");
                r = bdp_mcast_message(packet, packet_size);
                if (!r) {
                    return 0;
                }

                prev = now;
                num_info_requests_sent ++;

                timeout.tv_sec = (int)BDP_SECONDS_BETWEEN_PEER_INFO_REQUESTS;
                timeout.tv_usec = 1000 * 1000 * (BDP_SECONDS_BETWEEN_PEER_INFO_REQUESTS - (int)BDP_SECONDS_BETWEEN_PEER_INFO_REQUESTS);
            } else {
                double t = BDP_SECONDS_BETWEEN_PEER_INFO_REQUESTS - seconds_since_previous_packet;

                timeout.tv_sec = (int)t;
                timeout.tv_usec = 1000 * 1000 * (t - (int)t);
            }
        }

        printf("timeout after peer_info_request block: %d.%06d\n", (int)timeout.tv_sec, (int)timeout.tv_usec);


        FD_ZERO(&readers);
        FD_SET(bdp_mcast_socket, &readers);
        FD_SET(bdp_unicast_socket, &readers);
        if (bdp_mcast_socket > bdp_unicast_socket) {
            max_fd = bdp_mcast_socket;
        } else {
            max_fd = bdp_unicast_socket;
        }

        g_hash_table_foreach(bdp_peers, (GHFunc)bdp_find_next_timeout, &timeout);

        printf("bdp_browser_thread: entering select, timeout in %d.%06d seconds\n", (int)timeout.tv_sec, (int)timeout.tv_usec);
        r = select(max_fd + 1, &readers, NULL, NULL, &timeout);
        if (r < 0) {
            printf("bdp_browser_thread: error in select: %s\n", strerror(errno));
            bdp_mcast_socket = -1;  // FIXME: cleanup?
            return NULL;
        }

        if (r == 0) {
            printf("bdp_browser_thread: timeout, expire some peers\n");
            bdp_expire_peers();
            continue;
        }


        //
        // if we get here, then a peer sent a message to either our unicast or multicast socket
        // we try to read the unicast socket first
        //


        from_len = sizeof(from_addr);

        if (FD_ISSET(bdp_unicast_socket, &readers)) {
            packet_size = recvfrom(bdp_unicast_socket, packet, sizeof(packet), 0, (struct sockaddr *)&from_addr, &from_len);
            if (packet_size == -1) {
                printf("bdp_browser_thread: error in recvfrom: %s\n", strerror(errno));
                bdp_unicast_socket = -1;  // FIXME: cleanup?
                return NULL;
            }

            printf(
                "bdp_browser_thread: received a %lu-byte unicast packet from %s:%d\n",
                (unsigned long)packet_size,
                inet_ntoa(from_addr.sin_addr),
                ntohs(from_addr.sin_port)
            );
        } else {
            // must be mcast
            packet_size = recvfrom(bdp_mcast_socket, packet, sizeof(packet), 0, (struct sockaddr *)&from_addr, &from_len);
            if (packet_size == -1) {
                printf("bdp_browser_thread: error in recvfrom: %s\n", strerror(errno));
                bdp_mcast_socket = -1;  // FIXME: cleanup?
                return NULL;
            }

            printf(
                "bdp_browser_thread: received a %lu-byte multicast packet from %s:%d\n",
                (unsigned long)packet_size,
                inet_ntoa(from_addr.sin_addr),
                ntohs(from_addr.sin_port)
            );
        }

        bdp_hexdump(packet, packet_size);

        if (strncmp((char *)packet, "BDP Join", 8) == 0) {
            cal_peer_t *new_peer;
            bdp_peer_user_data_t *user_data;


            // 
            // make sure this is really a new peer
            // FIXME: handle this as per the design
            //

            if (g_hash_table_lookup(bdp_peers, &packet[20]) != NULL) {
                printf("bdp_browser_thread: got a Join for a peer already known\n");
                continue;
            }


            //
            // allocate all the memory we'll need
            //

            new_peer = (cal_peer_t *)calloc(1, sizeof(cal_peer_t));
            if (new_peer == NULL) {
                printf("bdp_browser_thread: error allocating memory for the new peer: %s\n", strerror(errno));
                continue;
            }

            user_data = (bdp_peer_user_data_t *)calloc(1, sizeof(bdp_peer_user_data_t));
            if (user_data == NULL) {
                printf("bdp_browser_thread: error allocating memory for peer user data: %s\n", strerror(errno));
                free(new_peer);
                continue;
            }


            // 
            // add the new peer to our list
            //

            new_peer->name = strdup((char *)&packet[20]);
            new_peer->num_unicast_addresses = 0;

            user_data->last_seen = time(NULL);
            user_data->port = ntohs(from_addr.sin_port);
            user_data->addr = from_addr.sin_addr;

            new_peer->user_data = user_data;

            g_hash_table_insert(bdp_peers, new_peer->name, new_peer);

            bdp_generate_join_event(new_peer);
        } 

        else if (strncmp((char *)packet, "BDP Heartbeat", 13) == 0) {
            cal_peer_t *peer;
            bdp_peer_user_data_t *user_data;


            // 
            // make sure this is really a new peer
            // FIXME: handle this as per the design
            //

            peer = g_hash_table_lookup(bdp_peers, &packet[25]);
            if (peer != NULL) {
                // got a heartbeat for a peer we already know
                user_data = peer->user_data;
                user_data->last_seen = time(NULL);
                continue;
            }


            //
            // if we get here, it's a heartbeat for a peer we don't know
            //


            //
            // allocate all the memory we'll need
            //

            peer = (cal_peer_t *)calloc(1, sizeof(cal_peer_t));
            if (peer == NULL) {
                printf("bdp_browser_thread: error allocating memory for the new peer: %s\n", strerror(errno));
                continue;
            }

            user_data = (bdp_peer_user_data_t *)calloc(1, sizeof(bdp_peer_user_data_t));
            if (user_data == NULL) {
                printf("bdp_browser_thread: error allocating memory for the new peer's user_data: %s\n", strerror(errno));
                free(peer);
                continue;
            }


            // 
            // add the new peer to our list
            //

            peer->name = strdup((char *)&packet[25]);
            peer->num_unicast_addresses = 0;

            user_data->last_seen = time(NULL);
            user_data->port = ntohs(from_addr.sin_port);
            user_data->addr = from_addr.sin_addr;

            peer->user_data = user_data;

            g_hash_table_insert(bdp_peers, peer->name, peer);

            bdp_generate_join_event(peer);
        } 

        else if (strncmp((char *)packet, "BDP Leave", 9) == 0) {
            cal_peer_t *peer;


            // 
            // make sure this is actually a known peer
            //

            if (bdp_peers == NULL) {
                printf("bdp_browser_thread: got a Leave for an unknown peer, ignoring\n");
                continue;
            }

            peer = g_hash_table_lookup(bdp_peers, &packet[21]);
            if (peer == NULL) {
                printf("bdp_browser_thread: got a Leave for an unknown peer, ignoring\n");
                continue;
            }

            bdp_generate_leave_event(peer);

            // remove the peer from our list of known peers
            free(peer->user_data);
            peer->user_data = NULL;
            g_hash_table_remove(bdp_peers, &packet[21]);
        } 

        else if (strncmp((char *)packet, "BDP Peer Info Reply", 19) == 0) {
            cal_peer_t *peer;
            bdp_peer_user_data_t *user_data;


            // 
            // make sure this is a new peer
            // FIXME: handle this as per the design
            //

            peer = g_hash_table_lookup(bdp_peers, &packet[31]);
            if (peer != NULL) {
                printf("bdp_browser_thread: got a Peer Info Reply for a peer already known\n");
                bdp_send_peer_info_ack(peer);
                continue;
            }


            //
            // if we get here it's a new peer
            //


            //
            // allocate all the memory we'll need
            //

            peer = (cal_peer_t *)calloc(1, sizeof(cal_peer_t));
            if (peer == NULL) {
                printf("bdp_browser_thread: error allocating memory for the new peer: %s\n", strerror(errno));
                continue;
            }

            user_data = (bdp_peer_user_data_t *)calloc(1, sizeof(bdp_peer_user_data_t));
            if (user_data == NULL) {
                printf("bdp_browser_thread: error allocating memory for peer user data: %s\n", strerror(errno));
                free(peer);
                continue;
            }


            // 
            // add the new peer to our list
            //

            peer->name = strdup((char *)&packet[31]);
            peer->num_unicast_addresses = 0;

            user_data->last_seen = time(NULL);
            user_data->port = ntohs(from_addr.sin_port);
            user_data->addr = from_addr.sin_addr;

            peer->user_data = user_data;

            g_hash_table_insert(bdp_peers, peer->name, peer);

            bdp_generate_join_event(peer);
            bdp_send_peer_info_ack(peer);
        } 

        else if (strncmp((char *)packet, "BDP Peer Info Request", 20) == 0) {
            // we ignore these
        } 

        else {
            // unknown event?
            printf("bdp_browser_thread: ignoring unknown packet!\n");
        }
    }
}

