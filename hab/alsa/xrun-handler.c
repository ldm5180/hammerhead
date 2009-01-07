
//
// Copyright (C) 2008-2009, Regents of the University of Colorado.
//


#include <alsa/asoundlib.h>
#include "hardware-abstractor.h"
#include "alsa-hab.h"


// overrun, underrun & suspend handler
int xrun_handler(snd_pcm_t *handle, int err) {
    if (err == -EPIPE) {
        static unsigned int underrun_count = 0;
        
        underrun_count ++;
        printf("underrun %d\n", underrun_count);

        err = snd_pcm_prepare(handle);
        if (err < 0) {
            printf("Can't re-prepare after underrun: %s\n", snd_strerror(err));
            return -1;
        }

#if 0
        err = snd_pcm_start(handle);
        if (err < 0) {
            printf("Can't re-start from underrun: %s\n", snd_strerror(err));
            return -1;
        }
#endif

        return 0;
    }

    printf("xrun!\n");
    if (err == -ESTRPIPE) {
        while ((err = snd_pcm_resume(handle)) == -EAGAIN) {
            // FIXME: dont sleep here
            g_usleep(1000*1000);   // wait until the suspend flag is released
        }
        if (err < 0) {
            err = snd_pcm_prepare(handle);
            if (err < 0) {
                printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
            }
        }
        return 0;
    }

    if (err == -EAGAIN) {
        // FIXME: should sleep a little here
        return 0;
    }

    return err;
}

