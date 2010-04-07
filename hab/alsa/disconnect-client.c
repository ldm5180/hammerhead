
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <alsa/asoundlib.h>

#include "alsa-hab.h"
#include "bionet-util.h"


static void disconnect_client_from_producer(bionet_stream_t *stream) {
    stream_info_t *sinfo;

    sinfo = bionet_stream_get_user_data(stream);

    if (sinfo->info.producer.num_clients <= 0) return;  // this should not happen...

    sinfo->info.producer.num_clients --;
    if (sinfo->info.producer.num_clients > 0) return;

    close_alsa_device(sinfo->info.producer.alsa);
    free(sinfo->info.producer.alsa);
    sinfo->info.producer.alsa = NULL;
}


static void disconnect_client_from_consumer(bionet_stream_t *stream, client_t *client) {
#if 0
    user_data_t *user_data;

    user_data = bionet_stream_get_user_data(stream);


    g_message(
        "client %s of Stream %s disconnects",
        client->id,
        bionet_stream_get_local_name(stream)
    );


    user_data->clients = g_slist_remove(user_data->clients, client);

    close_alsa_device(client->alsa);

    free(client->alsa);
    free(client);
#endif
}


void disconnect_client(bionet_stream_t *stream, client_t *client) {
    g_message("client disconnects from Stream %s", bionet_stream_get_local_name(stream));

    if (bionet_stream_get_direction(stream) == BIONET_STREAM_DIRECTION_PRODUCER) {
        disconnect_client_from_producer(stream);
    } else {
        disconnect_client_from_consumer(stream, NULL);
    }
}

