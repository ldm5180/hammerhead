
//
// Copyright (C) 2008-2009, Regents of the University of Colorado.
//


#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <sys/poll.h>
#include <glib.h>

#include "hardware-abstractor.h"
#include "alsa-hab.h"




GSList *nodes = NULL;
bionet_hab_t *this_hab = NULL;




#define Max(a,b) (((a) > (b)) ? (a) : (b))




// like memcmp, but for timevals
int tv_cmp(const struct timeval *t0, const struct timeval *t1) {
    if (t0->tv_sec < t1->tv_sec) return -1;
    if (t0->tv_sec > t1->tv_sec) return 1;
    if (t0->tv_usec < t1->tv_usec) return -1;
    if (t0->tv_usec > t1->tv_usec) return 1;
    return 0;
}


// t0 -= t1, but min is { 0, 0 }
void tv_sub(struct timeval *t0, const struct timeval *t1) {
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




void cb_stream_subscription(const char *client_id, const bionet_stream_t *stream) {
    printf("client '%s' subscribes to %s:%s\n", client_id, bionet_node_get_id(bionet_stream_get_node(stream)), bionet_stream_get_id(stream));
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

        FD_SET(bionet_fd, &readers);
        max_fd = bionet_fd;

#if 0
        // anyone new connecting to the streams?
        {
            GSList *ni;

            for (ni = nodes; ni != NULL; ni = ni->next) {
                bionet_node_t *node = ni->data;
                GSList *si;

                for (si = node->streams; si != NULL; si = si->next) {
                    bionet_stream_t *stream = si->data;
                    int socket = ((user_data_t*)(stream->user_data))->socket;

                    FD_SET(socket, &readers);
                    max_fd = Max(max_fd, socket);
                }
            }
        }

        // any of the connected streams want attention?
        {
            GSList *ni;

            for (ni = nodes; ni != NULL; ni = ni->next) {
                bionet_node_t *node = ni->data;
                GSList *si;

                for (si = node->streams; si != NULL; si = si->next) {
                    bionet_stream_t *stream = si->data;
                    user_data_t *user_data = stream->user_data;
                    GSList *ci;

                    for (ci = user_data->clients; ci != NULL; ci = ci->next) {
                        client_t *client = ci->data;

                        if (client->waiting == WAITING_FOR_CLIENT) {
                            FD_SET(client->socket, &readers);
                            max_fd = Max(max_fd, client->socket);
                        } else {
                            // FIXME: this works as long as there is 1 pollfd & it wants to be read....
                            FD_SET(client->alsa->pollfd->fd, &readers);
                            max_fd = Max(max_fd, client->alsa->pollfd->fd);
                        }
                    }
                }
            }
        }
#endif


#if 0
        printf("\n\n*****  top of main loop  *****\n");
        show_state();
#endif

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


#if 0
        //
        // any of the connected streams want attention?
        //
        {
            GSList *ni;

            for (ni = nodes; ni != NULL; ni = ni->next) {
                bionet_node_t *node = ni->data;
                GSList *si;

                for (si = node->streams; si != NULL; si = si->next) {
                    bionet_stream_t *stream = si->data;
                    user_data_t *user_data = stream->user_data;
                    GSList *ci;

                    for (ci = user_data->clients; ci != NULL; ci = ci->next) {
                        client_t *client = ci->data;
                        int client_disconnected;

                        if (client->waiting == WAITING_FOR_CLIENT) {
                            if (FD_ISSET(client->socket, &readers)) {
                                client_disconnected = handle_client(stream, client);
                                if (client_disconnected) break;
                            }
                        } else {
                            if (FD_ISSET(client->alsa->pollfd->fd, &readers)) {
                                client_disconnected = handle_client(stream, client);
                                if (client_disconnected) break;
                            }
                        }
                    }
                }
            }
        }


        //
        // anyone new connecting to the streams?
        //
        {
            GSList *ni;
            int got_a_new_client = 0;

            for (ni = nodes; ni != NULL; ni = ni->next) {
                bionet_node_t *node = ni->data;
                GSList *si;

                for (si = node->streams; si != NULL; si = si->next) {
                    bionet_stream_t *stream = si->data;
                    int socket = ((user_data_t*)(stream->user_data))->socket;

                    if (FD_ISSET(socket, &readers)) {
                        g_message("client connecting to %s:%s", node->id, stream->id);
                        connect_client(stream);
                        got_a_new_client = 1;
                    }
                }
            }

            if (got_a_new_client) {
                continue;
            }
        }
#endif

    }

    exit(0);
}

