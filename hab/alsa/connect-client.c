
//
// Copyright (C) 2008-2009, Regents of the University of Colorado.
//


#include "hardware-abstractor.h"
#include "alsa-hab.h"


void connect_client(bionet_stream_t *stream) {
    return;

/*
    user_data_t *user_data;
    client_t *new_client;
    int r;


    user_data = stream->user_data;


    new_client = (client_t *)calloc(1, sizeof(client_t));
    if (new_client == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "out of memory connecting client to %s!", stream->id);
        return;
    }

    new_client->socket = bionet_stream_accept(stream, ((user_data_t*)(stream->user_data))->socket);
    if (new_client->socket < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "error accepting connection on the stream %s", stream->id);
        free(new_client);
        return;
    }
    g_log("", G_LOG_LEVEL_DEBUG, "accepted a connection on the stream %s", stream->id);


    {
        snd_pcm_stream_t s;

        if (stream->direction == BIONET_STREAM_DIRECTION_PRODUCER) {
            s = SND_PCM_STREAM_CAPTURE;
            new_client->waiting = WAITING_FOR_ALSA;
        }  else {
            s = SND_PCM_STREAM_PLAYBACK;
            new_client->waiting = WAITING_FOR_CLIENT;
        }

        new_client->alsa = open_alsa_device(user_data->device, s);
        if (new_client->alsa == NULL) {
            g_log("", G_LOG_LEVEL_WARNING, "error opening ALSA device %s for %s", user_data->device, stream->id);
            close(new_client->socket);
            free(new_client);
            return;
        }
        g_log("", G_LOG_LEVEL_DEBUG, "opened ALSA device for %s", stream->id);
    }


    // add the pollfds of the new ALSA PCM
    new_client->alsa->num_pollfds = snd_pcm_poll_descriptors_count(new_client->alsa->pcm_handle);
    if (new_client->alsa->num_pollfds <= 0) {
        g_log("", G_LOG_LEVEL_WARNING, "invalid poll descriptors count: %d\n", new_client->alsa->num_pollfds);
        close(new_client->socket);
        free(new_client->alsa);
        free(new_client);
        return;
    }

    new_client->alsa->pollfd = (struct pollfd *)malloc(sizeof(struct pollfd) * new_client->alsa->num_pollfds);
    if (new_client->alsa->pollfd == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "error growing master pollfd list");
        close(new_client->socket);
        free(new_client->alsa);
        free(new_client);
        return;
    }

    r = snd_pcm_poll_descriptors(new_client->alsa->pcm_handle, new_client->alsa->pollfd, new_client->alsa->num_pollfds);
    if (r < 0) {
        printf("Unable to obtain poll descriptors for stream %s: %s\n", stream->id, snd_strerror(r));
        close(new_client->socket);
        free(new_client->alsa->pollfd);
        free(new_client->alsa);
        free(new_client);
        return;
    }


    // start the alsa pcm running
    r = snd_pcm_start(new_client->alsa->pcm_handle);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "error starting pcm: %s", snd_strerror(r));
        close(new_client->socket);
        free(new_client->alsa->pollfd);
        free(new_client->alsa);
        free(new_client);
        return;
    }


    // add the client to the streams list of clients
    user_data->clients = g_slist_prepend(user_data->clients, new_client);
*/
}

