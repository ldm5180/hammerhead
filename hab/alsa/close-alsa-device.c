
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <alsa/asoundlib.h>
#include "hardware-abstractor.h"
#include "alsa-hab.h"


void close_alsa_device(alsa_t *alsa) {
    snd_pcm_drain(alsa->pcm_handle);
    snd_pcm_close(alsa->pcm_handle);
    // FIXME: clean up memory allocated in the alsa_t
}

