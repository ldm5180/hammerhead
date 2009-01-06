
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


static void show_pcm(snd_pcm_t *pcm_handle) {
    switch (snd_pcm_stream(pcm_handle)) {
        case SND_PCM_STREAM_CAPTURE:
            g_log("", G_LOG_LEVEL_INFO, "                stream is CAPTURE");
            break;

        case SND_PCM_STREAM_PLAYBACK:
            g_log("", G_LOG_LEVEL_INFO, "                stream is PLAYBACK");
            break;

        default:
            g_log("", G_LOG_LEVEL_INFO, "                stream is ** Unknown! **");
            break;
    }

    switch (snd_pcm_state(pcm_handle)) {
        case SND_PCM_STATE_OPEN:
            g_log("", G_LOG_LEVEL_INFO, "                state is OPEN");
            break;

        case SND_PCM_STATE_SETUP:
            g_log("", G_LOG_LEVEL_INFO, "                state is SETUP");
            break;

        case SND_PCM_STATE_PREPARED:
            g_log("", G_LOG_LEVEL_INFO, "                state is PREPARED");
            break;

        case SND_PCM_STATE_RUNNING:
            g_log("", G_LOG_LEVEL_INFO, "                state is RUNNING");
            break;

        case SND_PCM_STATE_XRUN:
            g_log("", G_LOG_LEVEL_INFO, "                state is XRUN");
            break;

        case SND_PCM_STATE_DRAINING:
            g_log("", G_LOG_LEVEL_INFO, "                state is DRAINING");
            break;

        case SND_PCM_STATE_PAUSED:
            g_log("", G_LOG_LEVEL_INFO, "                state is PAUSED");
            break;

        case SND_PCM_STATE_SUSPENDED:
            g_log("", G_LOG_LEVEL_INFO, "                state is SUSPENDED");
            break;

        case SND_PCM_STATE_DISCONNECTED:
            g_log("", G_LOG_LEVEL_INFO, "                state is DISCONNECTED");
            break;

        default:
            g_log("", G_LOG_LEVEL_INFO, "                state is ** Unknown! **");
            break;
    }
}




void show_client(client_t *client) {
    g_log("", G_LOG_LEVEL_INFO, "                socket = %d", client->socket);
    g_log("", G_LOG_LEVEL_INFO, "                waiting for %s", (client->waiting == WAITING_FOR_ALSA) ? "ALSA" : "Client");
    show_pcm(client->alsa->pcm_handle);
}




static void show_clients(user_data_t *user_data) {
    GSList *ci;

    g_log("", G_LOG_LEVEL_INFO, "            %d clients", g_slist_length(user_data->clients));

    for (ci = user_data->clients; ci != NULL; ci = ci->next) {
        show_client((client_t *)ci->data);
    }
}




void show_state(void) {
    GSList *ni;

    g_log("", G_LOG_LEVEL_INFO, "internal state:");

    for (ni = nodes; ni != NULL; ni = ni->next) {
        bionet_node_t *node = ni->data;
        int si;

        g_message("    %s", bionet_node_get_id(node));

        // the streams
        for (si = 0; si < bionet_node_get_num_streams(node); si++) {
            bionet_stream_t *stream = bionet_node_get_stream_by_index(node, si);
            user_data_t *user_data = bionet_stream_get_user_data(stream);

            g_message(
                "        %s %s %s (%s)",
                bionet_stream_get_id(stream),
                bionet_stream_get_type(stream),
                bionet_stream_direction_to_string(bionet_stream_get_direction(stream)),
                user_data->device
            );

            show_clients(user_data);
        }
    }
}

