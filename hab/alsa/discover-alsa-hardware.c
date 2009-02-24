
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <ctype.h>
#include <string.h>
#include <alsa/asoundlib.h>

#include "alsa-hab.h"




/**
 * @brief Turns an Alsa device name into a Bionet Node ID.
 *
 * @param[in] input The Alsa device name
 *
 * @return The Node-ID, in a statically allocated char array.
 */
static const char *make_id(const char *input) {
    static char id[BIONET_NAME_COMPONENT_MAX_LEN];
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




static void try_add_producer_stream(bionet_node_t *node, int card, int device, snd_ctl_t *handle, snd_pcm_info_t *pcminfo) {
    char id[BIONET_NAME_COMPONENT_MAX_LEN];
    char alsa_device[256];
    int r;

    bionet_stream_t *stream;
    stream_info_t *sinfo;


    snd_pcm_info_set_stream(pcminfo, SND_PCM_STREAM_CAPTURE);
    snprintf(alsa_device, sizeof(alsa_device), "plug:dsnoop:%d", card);
    snprintf(id, sizeof(id), "%s", "Microphone");

    r = snd_ctl_pcm_info(handle, pcminfo);
    if (r < 0) {
        if (r != -ENOENT) {
            printf("control digital audio info (%i): %s\n", card, snd_strerror(r));
        }
        return;
    }

    stream = bionet_stream_new(node, id, BIONET_STREAM_DIRECTION_PRODUCER, "audio");
    if (stream == NULL) {
        printf("error creating new stream\n");
        exit(1);
    }

    sinfo = (stream_info_t *)calloc(1, sizeof(stream_info_t));
    if (sinfo == NULL) {
        printf("out of memory!");
        exit(1);
    }

    sinfo->device = strdup(alsa_device);
    if (sinfo->device == NULL) {
        printf("out of memory!");
        exit(1);
    }

    // the sinfo->info.producer fields are all initialized to zero by the calloc, and that's how i like it

    bionet_stream_set_user_data(stream, sinfo);

    r = bionet_node_add_stream(node, stream);
    if (r < 0) {
        printf("error adding stream to node\n");
        exit(1);
    }

    //  FIXME: set up mixer elements to sane defaults

    //  FIXME: add resources for mixer elements

}




static void try_add_consumer_stream(bionet_node_t *node, int card, int device, snd_ctl_t *handle, snd_pcm_info_t *pcminfo) {
    char id[BIONET_NAME_COMPONENT_MAX_LEN];
    char alsa_device[256];
    int r;

    bionet_stream_t *stream;
    stream_info_t *sinfo;


    snd_pcm_info_set_stream(pcminfo, SND_PCM_STREAM_PLAYBACK);
    snprintf(alsa_device, sizeof(alsa_device), "plug:dmix:%d", card);
    snprintf(id, sizeof(id), "%s", "Speaker");

    r = snd_ctl_pcm_info(handle, pcminfo);
    if (r < 0) {
        if (r != -ENOENT) {
            printf("control digital audio info (%i): %s\n", card, snd_strerror(r));
        }
        return;
    }

    stream = bionet_stream_new(node, id, BIONET_STREAM_DIRECTION_CONSUMER, "audio");
    if (stream == NULL) {
        printf("error creating new stream\n");
        exit(1);
    }

    sinfo = (stream_info_t *)calloc(1, sizeof(stream_info_t));
    if (sinfo == NULL) {
        printf("out of memory!");
        exit(1);
    }

    sinfo->device = strdup(alsa_device);
    if (sinfo->device == NULL) {
        printf("out of memory!");
        exit(1);
    }

    bionet_stream_set_user_data(stream, sinfo);

    r = bionet_node_add_stream(node, stream);
    if (r < 0) {
        printf("error adding stream to node\n");
        exit(1);
    }

    //  FIXME: set up mixer elements to sane defaults

    //  FIXME: add resources for mixer elements

}




static void try_add_streams(bionet_node_t *node, int card, int device, snd_ctl_t *handle, snd_pcm_info_t *pcminfo) {
    try_add_producer_stream(node, card, device, handle, pcminfo);
    try_add_consumer_stream(node, card, device, handle, pcminfo);
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
        int ni;

        for (ni = 0; ni < bionet_hab_get_num_nodes(this_hab); ni++) {
            bionet_node_t *node = bionet_hab_get_node_by_index(this_hab, ni);
            node_user_data_t *node_user_data = bionet_node_get_user_data(node);
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
        node_user_data_t *node_user_data;


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
            int ni;

            for (ni = 0; ni < bionet_hab_get_num_nodes(this_hab); ni++) {
                bionet_node_t *node = bionet_hab_get_node_by_index(this_hab, ni);
                node_user_data_t *node_user_data = bionet_node_get_user_data(node);

                if (strcmp(id, bionet_node_get_id(node)) == 0) {
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

        node_user_data = (node_user_data_t *)malloc(sizeof(node_user_data_t));
        if (node_user_data == NULL) {
            printf("error making user_data storage for new node\n");
            exit(1);
        }
        node_user_data->hardware_is_still_there = 1;

        bionet_node_set_user_data(node, node_user_data);


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
        // print a blurb about the new node
        //

        {
            int i;

            g_message("found a new audio device: %s", bionet_node_get_id(node));

            for (i = 0; i < bionet_node_get_num_streams(node); i ++) {
                bionet_stream_t *s = bionet_node_get_stream_by_index(node, i);
                stream_info_t *sinfo = bionet_stream_get_user_data(s);
                g_message("    %s (%s)", bionet_stream_get_id(s), sinfo->device);
            }
        }


        // 
        // Report this new Node to Bionet
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

        for (i = 0; i < bionet_hab_get_num_nodes(this_hab); i ++) {
            bionet_node_t *node = bionet_hab_get_node_by_index(this_hab, i);
            node_user_data_t *node_user_data = bionet_node_get_user_data(node);
            int j;

            if (node_user_data->hardware_is_still_there) {
                continue;
            }

            // this node has been lost
            printf("lost node %s\n", bionet_node_get_id(node));
            bionet_hab_remove_node_by_id(this_hab, bionet_node_get_id(node));
            hab_report_lost_node(bionet_node_get_id(node));

            free(node_user_data);
            bionet_node_set_user_data(node, NULL);

            // close open streams
            for (j = 0; j < bionet_node_get_num_streams(node); j ++) {
                bionet_stream_t *stream = bionet_node_get_stream_by_index(node, j);
                stream_info_t *sinfo = bionet_stream_get_user_data(stream);

                free(sinfo->device);

                if (bionet_stream_get_direction(stream) == BIONET_STREAM_DIRECTION_PRODUCER) {
                    if (sinfo->info.producer.alsa != NULL) {
                        close_alsa_device(sinfo->info.producer.alsa);
                        free(sinfo->info.producer.alsa);
                    }
                } else {
                    while (g_slist_length(sinfo->info.consumer.clients) > 0) {
                        client_t *client = g_slist_nth_data(sinfo->info.consumer.clients, 0);
                        sinfo->info.consumer.clients = g_slist_remove(sinfo->info.consumer.clients, client);

                        free(client->id);
                        close_alsa_device(client->alsa);
                        free(client->alsa);
                    }
                }

                free(sinfo);
                bionet_stream_set_user_data(stream, NULL);
            }

            bionet_node_free(node);

            i --;
        }
    }


    return 0;
}

