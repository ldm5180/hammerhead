
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


#include <errno.h>
#include <string.h>
#include <sys/poll.h>
#include <alsa/asoundlib.h>
#include "hardware-abstractor.h"
#include "alsa-hab.h"


int check_alsa_poll(alsa_t *alsa) {
    int r;
    unsigned short revents;

    r = poll(alsa->pollfd, alsa->num_pollfds, 0);

    if (r < 0) {
        g_warning("error polling alsa: %s", strerror(errno));
        return 0;
    }

    if (r == 0) {
        return 0;
    }

    snd_pcm_poll_descriptors_revents(alsa->pcm_handle, alsa->pollfd, alsa->num_pollfds, &revents);

    if (revents & POLLERR) {
        int state = snd_pcm_state(alsa->pcm_handle);

        switch (state) {

            case SND_PCM_STATE_PREPARED: {
                // "prepared" means "ready to start"
                return 1;
            }

            case SND_PCM_STATE_RUNNING: {
                // FIXME: why is this an error?  just not readable/writable I guess, but then why did it poll?
                // printf("speaker stream running\n");
                return 0;
            }

            case SND_PCM_STATE_XRUN: {
                printf("speaker stream xrun!\n");
                if (xrun_handler(alsa->pcm_handle, -EPIPE) < 0) {
                    printf("xrun error: %s\n", snd_strerror(-EPIPE));
                    exit(1);
                }
                // handled the error, poll it again
                return 0;
            }

            case SND_PCM_STATE_SUSPENDED: {
                printf("speaker stream suspended!\n");
                if (xrun_handler(alsa->pcm_handle, -ESTRPIPE) < 0) {
                    printf("xrun error: %s\n", snd_strerror(-ESTRPIPE));
                    exit(1);
                }
                // handled the error, poll it again
                return 0;
            }

            default: {
                g_log("", G_LOG_LEVEL_WARNING, "unhandled poll error, PCM state is %d", state);
                return 0;
            }
        }

    }

    if ((revents & POLLOUT) || (revents & POLLIN)) {
        // writable or readable!
        // printf("speaker stream readable/writable\n");
        return 1;
    }

    return 0;
}

