
//
// Copyright (C) 2008-2009, Regents of the University of Colorado.
//


#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include "hardware-abstractor.h"
#include "alsa-hab.h"




static ssize_t pcm_read(alsa_t *alsa) {
    int r;

#if 0
    if (sleep_min == 0 && count != chunk_size) {
        count = chunk_size;
    }
#endif

    r = snd_pcm_readi(alsa->pcm_handle, alsa->audio_buffer, alsa->audio_buffer_frames);
    if (r == alsa->audio_buffer_frames) {
        return r;
    } else if (r >= 0) {
        printf("short read from audio device (wanted %d frames, got %d)\n", alsa->audio_buffer_frames, r);
        return r;
    } else if (r == -EAGAIN) {
        return 0;
    } else if (r == -EPIPE) {
        xrun_handler(alsa->pcm_handle, r);
        return 0;
#if 0
    } else if (r == -ESTRPIPE) {
        suspend();
        return 0;
#endif

    } else {
        printf("unhandled read error: %s\n", snd_strerror(r));
        return -1;
    }
}




int handle_producer_client(bionet_stream_t *stream, client_t *client) {
    int r;
    int frames, bytes;

    if (client->waiting != WAITING_FOR_ALSA) {
        g_warning("waiting on client-of-producer?!");
        client->waiting = WAITING_FOR_ALSA;
        return 0;
    }

    if (!check_alsa_poll(client->alsa)) {
        return 0;
    }

    r = pcm_read(client->alsa);
    if (r < 0) {
        // FIXME
        printf("read error on stream %s\n", bionet_stream_get_local_name(stream));
        disconnect_client(stream, client);
        return 1;
    }

    frames = r;
    bytes = (frames * client->alsa->bytes_per_frame);

    r = write(client->socket, client->alsa->audio_buffer, bytes);
    if (r < 0) {
        printf("error writing stream %s to consumer: %s\n", bionet_stream_get_local_name(stream), strerror(errno));
        disconnect_client(stream, client);
        return 1;
    } else if (r < bytes) {
        printf("short write to stream %s consumer", bionet_stream_get_local_name(stream));
        disconnect_client(stream, client);
        return 1;
    }

    return 0;
}


int handle_consumer_client(bionet_stream_t *stream, client_t *client) {
    int r;
    int bytes_read, frames_read;


    if (
        (client->waiting == WAITING_FOR_ALSA) &&
        (!check_alsa_poll(client->alsa))
    ) {
        return 0;
    }


    //
    // If we get here, then either we were waiting for ALSA and ALSA is now
    // ready, or we were waiting for the client and the client is now ready.
    //


    bytes_read = read(client->socket, client->alsa->audio_buffer, (client->alsa->audio_buffer_frames * client->alsa->bytes_per_frame));
    if (bytes_read < 0) {
        if (errno == EAGAIN) {
            client->waiting = WAITING_FOR_CLIENT;
            return 0;
        }
        printf("error reading from producer for stream %s: %s\n", bionet_stream_get_local_name(stream), strerror(errno));
        disconnect_client(stream, client);
        return 1;
    } else if (bytes_read == 0) {
        g_message("eof reading from client of %s", bionet_stream_get_local_name(stream));
        disconnect_client(stream, client);
        return 1;
    } else {
        // we read some data, so now we should wait for alsa to consume it
        client->waiting = WAITING_FOR_ALSA;
    }

    // FIXME: this truncates to frame boundary, we should keep the frame fragment for later
    //    and then we lose sync
    frames_read = bytes_read / client->alsa->bytes_per_frame;

    // if everything's set up right, there's room for at least
    // this much data in ALSA's internal buffer, so we do it in
    // one write

    r = snd_pcm_writei(client->alsa->pcm_handle, client->alsa->audio_buffer, frames_read);
    if (r < 0) {
        if (xrun_handler(client->alsa->pcm_handle, r) < 0) {
            printf("Write error: %s\n", snd_strerror(r));
            disconnect_client(stream, client);
            return 1;
        }
    } else if (r != frames_read) {
        g_log("", G_LOG_LEVEL_WARNING, "short write to alsa device (wrote %d of %d frames)!", r, frames_read);
    }

    return 0;
}


int handle_client(bionet_stream_t *stream, client_t *client) {
    if (bionet_stream_get_direction(stream) == BIONET_STREAM_DIRECTION_PRODUCER) {
        return handle_producer_client(stream, client);
    } else {
        return handle_consumer_client(stream, client);
    }
}

