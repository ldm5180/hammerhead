
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




void producer_stream_subscription(const char *client_id, const bionet_stream_t *stream) {
    stream_info_t *sinfo;

    sinfo = bionet_stream_get_user_data(stream); 

    if (sinfo->info.producer.alsa == NULL) {
        int r;

        sinfo->info.producer.alsa = open_alsa_device(sinfo->device, SND_PCM_STREAM_CAPTURE);
        if (sinfo->info.producer.alsa == NULL) {
            g_warning("error opening ALSA device %s for %s", sinfo->device, bionet_stream_get_local_name(stream));
            goto fail0;
        }

        // add the pollfds of the new ALSA PCM
        sinfo->info.producer.alsa->num_pollfds = snd_pcm_poll_descriptors_count(sinfo->info.producer.alsa->pcm_handle);
        if (sinfo->info.producer.alsa->num_pollfds <= 0) {
            g_warning("invalid poll descriptors count: %d", sinfo->info.producer.alsa->num_pollfds);
            goto fail1;
        }

        sinfo->info.producer.alsa->pollfd = (struct pollfd *)malloc(sizeof(struct pollfd) * sinfo->info.producer.alsa->num_pollfds);
        if (sinfo->info.producer.alsa->pollfd == NULL) {
            g_warning("error growing master pollfd list");
            goto fail1;
        }

        r = snd_pcm_poll_descriptors(sinfo->info.producer.alsa->pcm_handle, sinfo->info.producer.alsa->pollfd, sinfo->info.producer.alsa->num_pollfds);
        if (r < 0) {
            g_warning("Unable to obtain poll descriptors for stream %s: %s", bionet_stream_get_local_name(stream), snd_strerror(r));
            goto fail2;
        }

        // start the alsa pcm running
        r = snd_pcm_start(sinfo->info.producer.alsa->pcm_handle);
        if (r < 0) {
            g_warning("error starting pcm: %s", snd_strerror(r));
            goto fail2;
        }
    }

    sinfo->info.producer.num_clients ++;

    return;


fail2:
    free(sinfo->info.producer.alsa->pollfd);

fail1:
    close_alsa_device(sinfo->info.producer.alsa);
    free(sinfo->info.producer.alsa);
    sinfo->info.producer.alsa = NULL;

fail0:
    return;
}




void consumer_stream_subscription(const char *client_id, const bionet_stream_t *stream) {
#if 0
    user_data = bionet_stream_get_user_data(stream); 

    new_client = (client_t *)calloc(1, sizeof(client_t));
    if (new_client == NULL) {
        g_warning("out of memory connecting client %s to %s!", client_id, bionet_stream_get_local_name(stream));
        goto fail0;
    }

    new_client->id = strdup(client_id);
    if (new_client->id == NULL) {
        g_warning("out of memory connecting client %s to %s!", client_id, bionet_stream_get_local_name(stream));
        goto fail1;
    }

    {
        snd_pcm_stream_t s;

        if (bionet_stream_get_direction(stream) == BIONET_STREAM_DIRECTION_PRODUCER) {
            s = SND_PCM_STREAM_CAPTURE;
            new_client->waiting = WAITING_FOR_ALSA;
        }  else {
            s = SND_PCM_STREAM_PLAYBACK;
            new_client->waiting = WAITING_FOR_CLIENT;
        }

        new_client->alsa = open_alsa_device(user_data->device, s);
        if (new_client->alsa == NULL) {
            g_warning("error opening ALSA device %s for %s", user_data->device, bionet_stream_get_local_name(stream));
            goto fail2;
        }
        g_debug("opened ALSA device for %s", bionet_stream_get_local_name(stream));
    }


    // add the pollfds of the new ALSA PCM
    new_client->alsa->num_pollfds = snd_pcm_poll_descriptors_count(new_client->alsa->pcm_handle);
    if (new_client->alsa->num_pollfds <= 0) {
        g_warning("invalid poll descriptors count: %d", new_client->alsa->num_pollfds);
        goto fail3;
    }

    new_client->alsa->pollfd = (struct pollfd *)malloc(sizeof(struct pollfd) * new_client->alsa->num_pollfds);
    if (new_client->alsa->pollfd == NULL) {
        g_warning("error growing master pollfd list");
        goto fail3;
    }

    r = snd_pcm_poll_descriptors(new_client->alsa->pcm_handle, new_client->alsa->pollfd, new_client->alsa->num_pollfds);
    if (r < 0) {
        g_warning("Unable to obtain poll descriptors for stream %s: %s", bionet_stream_get_local_name(stream), snd_strerror(r));
        goto fail4;
    }

    // start the alsa pcm running
    r = snd_pcm_start(new_client->alsa->pcm_handle);
    if (r < 0) {
        g_warning("error starting pcm: %s", snd_strerror(r));
        goto fail4;
    }

    // add the client to the streams list of clients
    user_data->clients = g_slist_prepend(user_data->clients, new_client);

    return;


fail4:
    free(new_client->alsa->pollfd);

fail3:
    close_alsa_device(new_client->alsa);
    free(new_client->alsa);

fail2:
    free(new_client->id);

fail1:
    free(new_client);

fail0:
    return;
#endif
}




void cb_stream_subscription(const char *client_id, const bionet_stream_t *stream) {
    printf("client '%s' subscribes to %s\n", client_id, bionet_stream_get_local_name(stream));

    if (bionet_stream_get_direction(stream) == BIONET_STREAM_DIRECTION_PRODUCER) {
        producer_stream_subscription(client_id, stream);
    } else {
        consumer_stream_subscription(client_id, stream);
    }
}

