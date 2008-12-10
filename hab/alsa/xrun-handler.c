
//
// Copyright (C) 2004-2008, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of GNU General Public License version 2, as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
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

