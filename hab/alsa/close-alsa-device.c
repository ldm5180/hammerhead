
//
// Copyright (C) 2008-2009, Regents of the University of Colorado.
//


#include <alsa/asoundlib.h>
#include "hardware-abstractor.h"
#include "alsa-hab.h"


void close_alsa_device(alsa_t *alsa) {
    snd_pcm_drain(alsa->pcm_handle);
    snd_pcm_close(alsa->pcm_handle);
}

