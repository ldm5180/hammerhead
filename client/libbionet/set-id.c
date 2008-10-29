
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>


#include "libbionet-internal.h"

#include "bionet.h"




int bionet_set_id(const char *new_id) {
    if (libbionet_client_id != NULL) free(libbionet_client_id);

    if (new_id == NULL) {
        libbionet_client_id = NULL;
        return 0;
    }

    libbionet_client_id = strdup(new_id);
    if (libbionet_client_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_set_id(): out of memory while setting Id!");
        return -1;
    }

    return 0;
}


