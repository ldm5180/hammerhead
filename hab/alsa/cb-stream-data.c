
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


void cb_stream_data(const char *client_id, bionet_stream_t *stream, const void *data, unsigned int size) {
    stream_info_t *si;
    client_t *this_client = NULL;
    int i;
    int num_frames;
    int r;

    printf("client '%s' sends %d bytes to stream %s\n", client_id, size, bionet_stream_get_local_name(stream));

    si = bionet_stream_get_user_data(stream);

    for (i = 0; i < g_slist_length(si->info.consumer.clients); i ++) {
        client_t *client = g_slist_nth_data(si->info.consumer.clients, i);
        if (strcmp(client_id, client->id) == 0) {
            this_client = client;
            break;
        }
    }

    if (this_client == NULL) {
        this_client = (client_t *)calloc(1, sizeof(client_t));
        if (this_client == NULL) {
            g_warning("out of memory!");
            return;
        }

        this_client->id = strdup(client_id);
        if (this_client->id == NULL) {
            g_warning("out of memory!");
            free(this_client);
            return;
        }

        this_client->alsa = open_alsa_device(si->device, SND_PCM_STREAM_PLAYBACK);
        if (this_client->alsa == NULL) {
            g_warning("error initializing alsa!");
            free(this_client->id);
            free(this_client);
            return;
        }

        // start the alsa pcm running
        r = snd_pcm_start(this_client->alsa->pcm_handle);
        if (r < 0) {
            g_warning("error starting pcm: %s", snd_strerror(r));
            close_alsa_device(this_client->alsa);
            free(this_client->id);
            free(this_client);
            return;
        }

        si->info.consumer.clients = g_slist_prepend(si->info.consumer.clients, this_client);
    }
    

    //
    // here, this_client is the client_t that's writing to the stream and it's all ready to go
    //


    #if 0
    if (!check_alsa_poll(this_client->alsa)) {
        g_warning("Alsa Stream is not ready to accept data from client");
        return;
    }
    #endif


    //
    // If we get here then ALSA is now ready to accept the data from this_client
    //


    // FIXME: this truncates to frame boundary, we should keep the frame
    // fragment for later and then we lose sync
    num_frames = size / this_client->alsa->bytes_per_frame;

    // if everything's set up right, there's room for at least
    // this much data in ALSA's internal buffer, so we do it in
    // one write

    r = snd_pcm_writei(this_client->alsa->pcm_handle, data, num_frames);
    if (r < 0) {
        if (xrun_handler(this_client->alsa->pcm_handle, r) < 0) {
            printf("Write error: %s\n", snd_strerror(r));
            exit(1);
            // disconnect_client(stream, client);
            // return 1;
        }
    } else if (r != num_frames) {
        g_warning("short write to alsa device (wrote %d of %d frames)!", r, num_frames);
    }
}

