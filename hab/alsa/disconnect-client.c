
//
// Copyright (C) 2008-2009, Regents of the University of Colorado.
//

#include <alsa/asoundlib.h>
#include "alsa-hab.h"
#include "bionet-util.h"

void disconnect_client(bionet_stream_t *stream, client_t *client) {
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
}

