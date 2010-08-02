
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(LINUX) || defined(MACOSX)
    #include <netdb.h>
    #include <pwd.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

#ifdef WINDOWS
    #include <winsock.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#include <glib.h>

#include "cal-server.h"

#include "libhab-internal.h"
#include "hardware-abstractor.h"


static void hash_element_datapoint_destroy (gpointer data);


int hab_connect(bionet_hab_t *hab) {
    const char *cal_name;


    // 
    // sanity checks
    //

    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_connect(): no HAB specified");
        return -1;
    }


    //
    // it we're already connected to Bionet, we're done
    //

    if (libhab_cal_handle != NULL) return cal_server.get_fd(libhab_cal_handle);


    //
    // If we get here we need to actually open the connection.
    //

    // record this hab
    libhab_this = hab;

    cal_name = bionet_hab_get_name(libhab_this);
    if (cal_name == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_connect(): error getting HAB name");
        libhab_this = NULL;
        return -1;
    }

    libhab_cal_handle = cal_server.init("bionet", cal_name, libhab_cal_callback, libhab_cal_topic_matches, libhab_ssl_ctx, libhab_require_security);
    if (libhab_cal_handle == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_connect(): error initializing CAL");
        libhab_this = NULL;
        return -1;
    }

    if (NULL != libhab_most_recently_published) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "hab_connect(): Most Recently Published Hash Table is already initialized.");
    } else {	
	bionet_pthread_mutex_lock(&published_hash_mutex);
	libhab_most_recently_published = g_hash_table_new_full(NULL, NULL,
							       NULL, hash_element_datapoint_destroy);
	bionet_pthread_mutex_unlock(&published_hash_mutex);
    }

    if (NULL == libhab_most_recently_published) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "hab_connect(): error initializing Most Recently Published hash table.");
    }

    return cal_server.get_fd(libhab_cal_handle);
}

static void hash_element_datapoint_destroy (gpointer data) {
    bionet_datapoint_free((bionet_datapoint_t *)data);
    return;
} /* hash_element_datapoint_destroy() */


#if 0

int hab_is_connected(void) {
    if (libhab_nag_nxio == NULL) {
        return 0;
    }

    if (libhab_nag_nxio->socket < 0) {
        return 0;
    }

    return 1;
}

#endif

