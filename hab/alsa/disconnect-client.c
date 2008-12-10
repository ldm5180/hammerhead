
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
#include "alsa-hab.h"


void disconnect_client(bionet_stream_t *stream, client_t *client) {
    user_data_t *user_data;

    user_data = stream->user_data;


    g_log("", G_LOG_LEVEL_INFO, "client on %s:%s (socket %d) disconnects", stream->node->id, stream->id, client->socket);


    user_data->clients = g_slist_remove(user_data->clients, client);

    close(client->socket);

    close_alsa_device(client->alsa);

    free(client->alsa);
    free(client);
}

