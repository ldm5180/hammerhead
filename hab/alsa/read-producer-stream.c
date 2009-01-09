
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




int read_producer_stream(bionet_stream_t *stream) {
    stream_info_t *sinfo = bionet_stream_get_user_data(stream);
    alsa_t *alsa = sinfo->info.producer.alsa;
    int r;
    int frames, bytes;

    if (!check_alsa_poll(alsa)) {
        return 0;
    }

    r = pcm_read(alsa);
    if (r < 0) {
        // FIXME
        printf("read error on stream %s\n", bionet_stream_get_local_name(stream));
        return 1;
    }

    frames = r;
    bytes = (frames * alsa->bytes_per_frame);

    hab_publish_stream(stream, alsa->audio_buffer, bytes);

#if 0
    FIXME
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
#endif

    return 0;
}


