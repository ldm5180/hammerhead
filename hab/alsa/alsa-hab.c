
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <sys/poll.h>
#include <glib.h>

#include "hardware-abstractor.h"
#include "alsa-hab.h"




bionet_hab_t *this_hab = NULL;




#define Max(a,b) (((a) > (b)) ? (a) : (b))




// like memcmp, but for timevals
static int tv_cmp(const struct timeval *t0, const struct timeval *t1) {
    if (t0->tv_sec < t1->tv_sec) return -1;
    if (t0->tv_sec > t1->tv_sec) return 1;
    if (t0->tv_usec < t1->tv_usec) return -1;
    if (t0->tv_usec > t1->tv_usec) return 1;
    return 0;
}


// t0 -= t1, but min is { 0, 0 }
static void tv_sub(struct timeval *t0, const struct timeval *t1) {
    if (tv_cmp(t0, t1) <= 0) {
        t0->tv_sec = 0;
        t0->tv_usec = 0;
        return;
    }
    t0->tv_usec -= t1->tv_usec;
    if (t0->tv_usec < 0) {
        t0->tv_usec += 1000000;
        t0->tv_sec --;
    }
    t0->tv_sec -= t1->tv_sec;
}




void cb_lost_client(const char *client_id) {
    // FIXME: in the future this will close all the client's Alsa connections
}




int main(int argc, char *argv[]) {
    int r;

    int bionet_fd;

    struct timeval start_of_cycle;


    g_log_set_default_handler(bionet_glib_log_handler, NULL);


    //
    //  Initialize the HAB & connect to Bionet
    //

    hab_register_callback_lost_client(cb_lost_client);
    hab_register_callback_stream_subscription(cb_stream_subscription);
    hab_register_callback_stream_unsubscription(cb_stream_unsubscription);
    hab_register_callback_stream_data(cb_stream_data);

    this_hab = bionet_hab_new("Alsa", NULL);

    bionet_fd = hab_connect(this_hab);
    if (bionet_fd < 0) {
        printf("problem connecting to Bionet, exiting\n");
        return 1;
    }


    //
    //  Initialize the ALSA stuff
    //

    r = discover_alsa_hardware();
    if (r < 0) {
        g_critical("no ALSA hardware found!");
        exit(1);
    }

    r = gettimeofday(&start_of_cycle, NULL);
    if (r < 0) {
        g_critical("error with gettimeofday: %s", strerror(errno));
        exit(1);
    }


    //  
    // main loop
    //

    while (1) {
        fd_set readers;
        int max_fd;
        struct timeval timeout;


        {
            struct timeval now;
            struct timeval time_since_last_rediscovery;
            struct timeval time_between_rediscoveries = { 5, 0 };

            r = gettimeofday(&now, NULL);
            if (r < 0) {
                g_critical("error with gettimeofday: %s", strerror(errno));
                exit(1);
            }

            time_since_last_rediscovery = now;
            tv_sub(&time_since_last_rediscovery, &start_of_cycle);

            if (tv_cmp(&time_since_last_rediscovery, &time_between_rediscoveries) >= 0) {
                discover_alsa_hardware();
                start_of_cycle = now;
                timeout = time_between_rediscoveries;
            } else {
                timeout = time_between_rediscoveries;
                tv_sub(&timeout, &time_since_last_rediscovery);
            }

            // printf("start of cycle: %d.%06d\n", (int)start_of_cycle.tv_sec, (int)start_of_cycle.tv_usec);
            // printf("now: %d.%06d\n", (int)now.tv_sec, (int)now.tv_usec);
            // printf("timeout: %d.%06d\n", (int)timeout.tv_sec, (int)timeout.tv_usec);
        }


        FD_ZERO(&readers);


        // does Bionet want attention?
        FD_SET(bionet_fd, &readers);
        max_fd = bionet_fd;


        // do any of the producer streams want attention?
        {
            int ni;

            for (ni = 0; ni < bionet_hab_get_num_nodes(this_hab); ni++) {
                bionet_node_t *node = bionet_hab_get_node_by_index(this_hab, ni);
                int si;

                for (si = 0; si < bionet_node_get_num_streams(node); si++) {
                    bionet_stream_t *stream = bionet_node_get_stream_by_index(node, si);
                    stream_info_t *sinfo = bionet_stream_get_user_data(stream);

                    if ((bionet_stream_get_direction(stream) == BIONET_STREAM_DIRECTION_PRODUCER) && (sinfo->info.producer.alsa != NULL)) {
                        FD_SET(sinfo->info.producer.alsa->pollfd->fd, &readers);
                        max_fd = Max(max_fd, sinfo->info.producer.alsa->pollfd->fd);
                    }
                }
            }
        }

        // printf("\n\n*****  top of main loop  *****\n");
        // show_state();

        r = select(max_fd + 1, &readers, NULL, NULL, &timeout);
        if (r < 0) {
            if (errno == EINTR) {
                continue;
            }
            printf("error from select: %s\n", strerror(errno));
            exit(1);
        }


        // anything from Bionet?
        if (FD_ISSET(bionet_fd, &readers)) {
            hab_read();
        }


        //
        // any of the connected producer streams want attention?
        //
        {
            int ni;

            for (ni = 0; ni < bionet_hab_get_num_nodes(this_hab); ni++) {
                bionet_node_t *node = bionet_hab_get_node_by_index(this_hab, ni);
                int si;

                for (si = 0; si < bionet_node_get_num_streams(node); si++) {
                    bionet_stream_t *stream = bionet_node_get_stream_by_index(node, si);
                    stream_info_t *sinfo = bionet_stream_get_user_data(stream);

                    if ((bionet_stream_get_direction(stream) == BIONET_STREAM_DIRECTION_PRODUCER) && (sinfo->info.producer.alsa != NULL)) {
                        if (FD_ISSET(sinfo->info.producer.alsa->pollfd->fd, &readers)) {
                            int client_disconnected = read_producer_stream(stream);
                            if (client_disconnected) break;
                        }
                    }
                }
            }
        }

    }

    exit(0);
}

