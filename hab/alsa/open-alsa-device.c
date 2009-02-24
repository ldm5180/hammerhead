
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <alsa/asoundlib.h>
#include "hardware-abstractor.h"
#include "alsa-hab.h"


static int set_params(alsa_t *alsa) {
    int r;

    snd_pcm_hw_params_t *hw_params;
    snd_pcm_sw_params_t *sw_params;


    // 
    // deal with the HW params
    //

    snd_pcm_hw_params_alloca(&hw_params);


    // get current hardware parameters
    r = snd_pcm_hw_params_any(alsa->pcm_handle, hw_params);
    if (r < 0) {
        printf("Broken configuration for playback: no configurations available: %s\n", snd_strerror(r));
        return -1;
    }

    // enable hardware resampling (rate-changing)
    r = snd_pcm_hw_params_set_rate_resample(alsa->pcm_handle, hw_params, 1);
    if (r < 0) {
        printf("Resampling setup failed for playback: %s\n", snd_strerror(r));
        return -1;
    }

    // set the read/write format to interleaved 
    r = snd_pcm_hw_params_set_access(alsa->pcm_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (r < 0) {
        printf("Access type not available for playback: %s\n", snd_strerror(r));
        return -1;
    }

    // set the sample format
    r = snd_pcm_hw_params_set_format(alsa->pcm_handle, hw_params, ALSA_FORMAT);
    if (r < 0) {
        printf("Sample format not available for playback: %s\n", snd_strerror(r));
        return -1;
    }

    // set the number of channels
    r = snd_pcm_hw_params_set_channels(alsa->pcm_handle, hw_params, ALSA_CHANNELS);
    if (r < 0) {
        printf("Couldnt set channels count to %u: %s\n", ALSA_CHANNELS, snd_strerror(r));
        return -1;
    }

    // try to set the sample rate - may not get what we want
    {
        unsigned int actual_rate = ALSA_RATE;

        r = snd_pcm_hw_params_set_rate_near(alsa->pcm_handle, hw_params, &actual_rate, 0);
        if (r < 0) {
            printf("Rate %i Hz not available for playback: %s\n", ALSA_RATE, snd_strerror(r));
            return -1;
        }
        if (actual_rate != ALSA_RATE) {
            printf("Rate doesn't match (requested %u Hz, got %u Hz)\n", ALSA_RATE, actual_rate);
            return -1;
        }
    }


    //
    // deal with the buffer time
    //

    {
        unsigned int min, max;
        int dir;

        dir = 0;
        min = 0;

        r = snd_pcm_hw_params_get_buffer_time_min(hw_params, &min, &dir);
        if (r < 0) {
            printf("Unable to get buffer time min for playback: %s\n", snd_strerror(r));
            return -1;
        }

        dir = 0;
        max = 0;

        r = snd_pcm_hw_params_get_buffer_time_max(hw_params, &max, &dir);
        if (r < 0) {
            printf("Unable to get buffer time max for playback: %s\n", snd_strerror(r));
            return -1;
        }

        dir = -1;
        alsa->buffer_time_us = ALSA_BUFFER_TIME_US;
        r = snd_pcm_hw_params_set_buffer_time_near(alsa->pcm_handle, hw_params, &alsa->buffer_time_us, &dir);
        if (r < 0) {
            printf("Unable to set buffer time for playback to %i: %s\n", ALSA_BUFFER_TIME_US, snd_strerror(r));
            return -1;
        }

        r = snd_pcm_hw_params_get_buffer_size(hw_params, &alsa->buffer_size);
        if (r < 0) {
            printf("Unable to get buffer size for playback: %s\n", snd_strerror(r));
            return -1;
        }

        printf(
            "playback buffer: min=%i us, max=%d us, asked for %i us, got %i us (%lu frames)\n",
            min,
            max,
            ALSA_BUFFER_TIME_US,
            alsa->buffer_time_us,
            (unsigned long)alsa->buffer_size
        );
    }


    //
    // set the period time
    //

    {
        unsigned int min, max;
        int dir;

        dir = 0;
        min = 0;

        r = snd_pcm_hw_params_get_period_time_min(hw_params, &min, &dir);
        if (r < 0) {
            printf("Unable to get period time min for playback: %s\n", snd_strerror(r));
            return -1;
        }

        dir = 0;
        max = 0;

        r = snd_pcm_hw_params_get_period_time_max(hw_params, &max, &dir);
        if (r < 0) {
            printf("Unable to get period time max for playback: %s\n", snd_strerror(r));
            return -1;
        }

        dir = -1;
        alsa->period_time_us = ALSA_PERIOD_TIME_US;
        r = snd_pcm_hw_params_set_period_time_near(alsa->pcm_handle, hw_params, &alsa->period_time_us, &dir);
        if (r < 0) {
            printf("Unable to set period time for playback to %i: %s\n", ALSA_PERIOD_TIME_US, snd_strerror(r));
            return -1;
        }

        dir = 0;
        r = snd_pcm_hw_params_get_period_size(hw_params, &alsa->period_size, &dir);
        if (r < 0) {
            printf("Unable to get period size for playback: %s\n", snd_strerror(r));
            return -1;
        }

        // printf("period time = %u, period size = %u\n", alsa->period_time_us, (unsigned int)alsa->period_size);
        printf(
            "playback period: min=%i us, max=%d us, asked for %i us, got %i us (%lu frames)\n",
            min,
            max,
            ALSA_PERIOD_TIME_US,
            alsa->period_time_us,
            (unsigned long)alsa->period_size
        );
    }


    // write the updated parameters to device
    r = snd_pcm_hw_params(alsa->pcm_handle, hw_params);
    if (r < 0) {
        printf("Unable to set hw params for playback: %s\n", snd_strerror(r));
        return -1;
    }




    // 
    // deal with the SW params
    //

    snd_pcm_sw_params_alloca(&sw_params);


    // get the current sw_params
    r = snd_pcm_sw_params_current(alsa->pcm_handle, sw_params);
    if (r < 0) {
        printf("Unable to determine current sw_params for playback: %s\n", snd_strerror(r));
        return -1;
    }

    r = snd_pcm_sw_params_set_avail_min(alsa->pcm_handle, sw_params, alsa->period_size);
    if (r < 0) {
        printf("Unable to set avail_min: %s\n", snd_strerror(r));
        return -1;
    }

    r = snd_pcm_sw_params_set_start_threshold(alsa->pcm_handle, sw_params, 120);
    if (r < 0) {
        printf("Unable to set start_threshold: %s\n", snd_strerror(r));
        return -1;
    }

    r = snd_pcm_sw_params_set_stop_threshold(alsa->pcm_handle, sw_params, alsa->buffer_size);
    if (r < 0) {
        printf("Unable to set stop_threshold: %s\n", snd_strerror(r));
        return -1;
    }

    r = snd_pcm_sw_params(alsa->pcm_handle, sw_params);
    if (r < 0) {
        printf("unable to install sw params: %s\n", snd_strerror(r));
        // snd_pcm_sw_params_dump(sw_params, log);
        return -1;
    }

    alsa->bytes_per_frame = ALSA_CHANNELS * (snd_pcm_format_width(ALSA_FORMAT) / 8);
    alsa->audio_buffer_frames = alsa->period_size;
    alsa->audio_buffer = malloc(alsa->audio_buffer_frames * alsa->bytes_per_frame);
    if (alsa->audio_buffer == NULL) {
        printf("set_params(): out of memory\n");
        free(alsa);
        return -1;
    }

    return 0;
}




#define DEBUG_ALSA 0
// #define DEBUG_ALSA 1


alsa_t *open_alsa_device(char *device, snd_pcm_stream_t direction) {
    alsa_t *alsa;

    int r;


    alsa = (alsa_t *)calloc(1, sizeof(alsa_t));
    if (alsa == NULL) {
        printf("out of memory!");
        return NULL;
    }


    // open the device in the requested mode (playback or capture)
    r = snd_pcm_open(&alsa->pcm_handle, device, direction, SND_PCM_NONBLOCK);
    if (r < 0) {
        printf("snd_pcm_open of '%s' fails: %s\n", device, snd_strerror(r));
        free(alsa);
        return NULL;
    }

    r = set_params(alsa);
    if (r < 0) {
        printf("Setting params failed: %s\n", snd_strerror(r));
        free(alsa);
        return NULL;
    }


#if DEBUG_ALSA
    //
    // this sends alsa-lib's logging to stdout
    //

    {
        snd_output_t *output = NULL;

        r = snd_output_stdio_attach(&output, stdout, 0);
        if (r < 0) {
            printf("Output failed: %s\n", snd_strerror(r));
            free(alsa);
            return NULL;
        }

        printf("playback '%s' buffer size: %d us (%d frames)\n", device, alsa->buffer_time_us, (int)alsa->buffer_size);
        printf("playback '%s' period size: %d us (%d frames)\n", device, alsa->period_time_us, (int)alsa->period_size);

        // print ALSA settings for the speaker's handle
        printf("final alsa settings for '%s' handle\n", device);
        snd_pcm_dump(alsa->pcm_handle, output);
    }
#endif

    return alsa;
}


