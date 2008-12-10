
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


#define NDEBUG  // turn off asserts globally, to shut up a false-positive one in the alsa library that causes our -Werror to fail...

#include <ctype.h>
#include <string.h>
#include <alsa/asoundlib.h>

#include "alsa-hab.h"




static const char *make_id(const char *input) {
    static char id[256];
    char *p;

    strncpy(id, input, sizeof(id));
    id[sizeof(id) - 1] = '\0';

    // convert any invalid characters to '-'
    for (p = id; *p != '\0'; p ++) { 
        if (!isalnum(*p) && (*p != '-')) {
            *p = '-';
        }
    }

    // strip any trailing '-' characters
    // (needed for the Logitech USB microphone)
    while (id[strlen(id) - 1] == '-') {
        id[strlen(id) - 1] = '\0';
    }

    return id;
}



static void try_add_stream(bionet_node_t *node, int card, int device, snd_ctl_t *handle, snd_pcm_info_t *pcminfo, bionet_stream_direction_t dir) {
    char id[256];
    char alsa_device[256];
    int r;

    bionet_stream_t *stream;


    if (dir == BIONET_STREAM_DIRECTION_PRODUCER) {
        snd_pcm_info_set_stream(pcminfo, SND_PCM_STREAM_CAPTURE);
        snprintf(alsa_device, sizeof(alsa_device), "plug:dsnoop:%d", card);
        snprintf(id, sizeof(id), "%s", "Microphone");
    } else {
        snd_pcm_info_set_stream(pcminfo, SND_PCM_STREAM_PLAYBACK);
        snprintf(alsa_device, sizeof(alsa_device), "plug:dmix:%d", card);
        snprintf(id, sizeof(id), "%s", "Speaker");
    }

    r = snd_ctl_pcm_info(handle, pcminfo);
    if (r < 0) {
        if (r != -ENOENT) {
            printf("control digital audio info (%i): %s\n", card, snd_strerror(r));
        }
        return;
    }

    stream = bionet_stream_new(node, id, dir, "audio", NULL, 0);
    if (stream == NULL) {
        printf("error creating new stream\n");
        exit(1);
    }


    {
        user_data_t *user_data;

        user_data = (user_data_t *)calloc(1, sizeof(user_data_t));
        if (user_data == NULL) {
            printf("out of memory!");
            exit(1);
        }

        user_data->device = strdup(alsa_device);
        if (user_data->device == NULL) {
            printf("out of memory!");
            exit(1);
        }

        stream->user_data = user_data;
    }


    r = bionet_stream_listen(stream);
    if (r < 0) {
        printf("error starting Mic stream listening\n");
        exit(1);
    }

    ((user_data_t*)(stream->user_data))->socket = r;

    r = bionet_node_add_stream(node, stream);
    if (r < 0) {
        printf("error adding stream to node\n");
        exit(1);
    }


    //  FIXME: set up mixer elements to sane defaults

    //  FIXME: add resources for mixer elements

}




static void try_add_streams(bionet_node_t *node, int card, int device, snd_ctl_t *handle, snd_pcm_info_t *pcminfo) {
    try_add_stream(node, card, device, handle, pcminfo, BIONET_STREAM_DIRECTION_PRODUCER);
    try_add_stream(node, card, device, handle, pcminfo, BIONET_STREAM_DIRECTION_CONSUMER);
}




int discover_alsa_hardware(void) {
    snd_ctl_t *handle;
    int card, dev;
    snd_ctl_card_info_t *info;
    snd_pcm_info_t *pcminfo;

    int r;


    // 
    // First we mark all our nodes (known soundcards) as missing.  We'll
    // mark them present later if we actually find them this time around.
    //

    {
        GSList *ni;

        for (ni = nodes; ni != NULL; ni = ni->next) {
            bionet_node_t *node = ni->data;
            node_user_data_t *node_user_data = node->user_data;

            node_user_data->hardware_is_still_there = 0;
        }
    }


    snd_ctl_card_info_alloca(&info);
    snd_pcm_info_alloca(&pcminfo);


    card = -1;
    r = snd_card_next(&card);
    if (r < 0) {
        g_warning("snd_card_next error: %s\n", snd_strerror(r));
        return 0;
    }


    while (card >= 0) {
        char hw_name[32];

        const char *id;
        bionet_node_t *node;


        r = snprintf(hw_name, sizeof(hw_name), "hw:%d", card);
        if (r >= sizeof(hw_name)) {
            g_warning("hardware name is too long!\n");
            goto next_card;
        }

        r = snd_ctl_open(&handle, hw_name, 0);
        if (r < 0) {
            g_warning("error opening control for '%s': %s\n", hw_name, snd_strerror(r));
            goto next_card;
        }

        r = snd_ctl_card_info(handle, info);
        if (r < 0) {
            g_warning("error getting control info for '%s': %s\n", hw_name, snd_strerror(r));
            snd_ctl_close(handle);
            goto next_card;
        }

        id = make_id(snd_ctl_card_info_get_name(info));


        // 
        // do we already know about this card?
        //

        {
            GSList *ni;

            for (ni = nodes; ni != NULL; ni = ni->next) {
                bionet_node_t *node = ni->data;
                node_user_data_t *node_user_data = node->user_data;

                if (strcmp(id, node->id) == 0) {
                    node_user_data->hardware_is_still_there = 1;
                    snd_ctl_close(handle);
                    goto next_card;
                }
            }
        }


        // 
        // found a new card!
        // make a node for this card
        //

        node = bionet_node_new(this_hab, id);
        if (node == NULL) {
            printf("error making new node!\n");
            exit(1);
        }

        bionet_hab_add_node(this_hab, node);

        node->user_data = (node_user_data_t *)malloc(sizeof(node_user_data_t));
        if (node->user_data == NULL) {
            printf("error making user_data storage for new node\n");
            exit(1);
        }

        ((node_user_data_t *)node->user_data)->hardware_is_still_there = 1;


        // for now we just do device 0
        // FIXME: loop over all the devices & subdevices

        dev = 0;

        snd_pcm_info_set_device(pcminfo, dev);
        snd_pcm_info_set_subdevice(pcminfo, 0);

        try_add_streams(node, card, dev, handle, pcminfo);

#if 0
        dev = -1;

        // loop over all the devices on this card, adding all the device's streams
        while (1) {
            int idx;
            unsigned int count;

            r = snd_ctl_pcm_next_device(handle, &dev);
            if (r < 0) {
                g_warning("snd_ctl_pcm_next_device error: %s", snd_strerror(r));
                continue;
            }

            if (dev < 0) {
                // last device
                break;
            }

            printf("    dev: %i\n", dev);

            snd_pcm_info_set_device(pcminfo, dev);
            snd_pcm_info_set_subdevice(pcminfo, 0);


            try_add_streams(node, card, dev, handle, pcminfo);


            count = snd_pcm_info_get_subdevices_count(pcminfo);

            printf("    Subdevices: %i/%i\n", snd_pcm_info_get_subdevices_avail(pcminfo), count);
            for (idx = 0; idx < (int)count; idx++) {
                snd_pcm_info_set_subdevice(pcminfo, idx);
                if ((r = snd_ctl_pcm_info(handle, pcminfo)) < 0) {
                    printf("control digital audio playback info (%i): %s\n", card, snd_strerror(r));
                } else {
                    printf("        Subdevice #%i: %s\n", idx, snd_pcm_info_get_subdevice_name(pcminfo));
                }
            }
        }
#endif

        snd_ctl_close(handle);


        //
        // add the node to the node-list
        //

        nodes = g_slist_prepend(nodes, node);


        // 
        // print a blurb about the new node
        //

        {
            GSList *sp;

            g_message("found a new audio device: %s", node->id);

            for (sp = node->streams; sp != NULL; sp = sp->next) {
                bionet_stream_t *s = sp->data;
                user_data_t *user_data = s->user_data;
                g_message("    %s (%s)", s->id, user_data->device);
            }
        }


        // 
        // Report this new Node to the NAG
        //

        r = hab_report_new_node(node);
        if (r < 0) {
            printf("error reporting new node!\n");
            exit(1);
        }

next_card:
        if (snd_card_next(&card) < 0) {
            printf("snd_card_next error\n");
            break;
        }
    }


    // 
    // Any nodes (sound cards) that were lost (ie unplugged), we close any
    // open streams and remove the node.
    //

    {
        int i;

        for (i = 0; i < g_slist_length(nodes); i ++) {
            bionet_node_t *node = g_slist_nth_data(nodes, i);
            node_user_data_t *node_user_data = node->user_data;
            int j;

            if (node_user_data->hardware_is_still_there) {
                continue;
            }

            // this node has been lost
            printf("lost node %s\n", node->id);
            hab_report_lost_node(node->id);

            free(node->user_data);
            node->user_data = NULL;

            // close open streams
            for (j = 0; j < g_slist_length(node->streams); j ++) {
                bionet_stream_t *stream = g_slist_nth_data(node->streams, j);
                user_data_t *user_data = stream->user_data;

                free(user_data->device);

                while (user_data->clients != NULL) {
                    client_t *client = g_slist_nth_data(user_data->clients, 0);

                    disconnect_client(stream, client);
                }

                free(stream->user_data);
                stream->user_data = NULL;
            }

            nodes = g_slist_remove(nodes, node);
            i --;

            bionet_node_free(node);
        }
    }


    return 0;
}


