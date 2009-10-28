
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#if defined(LINUX) || defined(MACOSX)
    #include <netdb.h>
    #include <pwd.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

#include <glib.h>

#include "cal-client.h"
#include "bdm-client.h"
#include "bdm-util.h"
#include "libbdm-internal.h"


int bdm_is_connected() {
    if (libbdm_cal_fd < 0) {
        return 0;
    }

    return 1;
}




#if 0
static const char *libbdm_get_id(void) {
    struct passwd *passwd;
    struct hostent *host;
    struct sockaddr_in local_socket;
    char program_name[512];
    static char id[1024];


    // get the user name
    passwd = getpwuid(getuid());
    if (passwd == NULL) {
	g_log(
	    BIONET_LOG_DOMAIN,
	    G_LOG_LEVEL_WARNING,
	    "bdm_connect_to_nag(): unable to determine user name for ident string: %s",
	    strerror(errno)
	);
	return NULL;
    }


    // get the hostent for the local host
    host = gethostbyname("localhost");
    if (host == NULL) {
	g_log(
	    BIONET_LOG_DOMAIN,
	    G_LOG_LEVEL_WARNING,
	    "bdm_connect_to_nag(): unable to gethostbyname(\"localhost\") for ident string (%s), oh well",
	    hstrerror(h_errno)
	);
    }


    // get the local socket port number
    {
	int r;
	socklen_t len;

	len = sizeof(struct sockaddr_in);
	r = getsockname(libbdm_nag_nxio->socket, (struct sockaddr *)&local_socket, &len);
	if (r < 0) {
	    g_log(
		BIONET_LOG_DOMAIN,
		G_LOG_LEVEL_WARNING,
		"bdm_connect_to_nag(): error getting local socket name: %s",
		strerror(errno)
	    );
	    return NULL;
	}
    }


    // get the program name
#ifdef LINUX
    {
	int fd;
	char *tmp;
        int r;


	fd = open("/proc/self/cmdline", O_RDONLY);
	if (fd < 0) {
	    g_log(
		BIONET_LOG_DOMAIN,
		G_LOG_LEVEL_WARNING,
		"bdm_connect_to_nag(): error opening /proc/self/cmdline: %s",
		strerror(errno)
	    );
	    return NULL;
	}

        r = read(fd, program_name, sizeof(program_name) - 1);
        close(fd);

        if (r <= 0) {
            g_log(
                BIONET_LOG_DOMAIN,
                G_LOG_LEVEL_WARNING,
                "bdm_connect_to_nag(): error reading /proc/self/cmdline: %s",
                strerror(errno)
            );
            return NULL;
        }

        program_name[r] = '\0';  // this is redundant, because the /proc file already contains the NUL, but it makes Coverity happy

        while ((tmp = memchr(program_name, '/', strlen(program_name))) != NULL) {
            int new_len = strlen(tmp+1);
            memmove(program_name, tmp+1, new_len);
            program_name[new_len] = '\0';
        }
    }
#endif

#ifdef MACOSX
    {
        char * name = (char *)getprogname();
    	strncpy(program_name, name, 512);
    }
#endif

    snprintf(
	id, 
	sizeof(id),
	"%s@%s:%d (%s [%d])",
	passwd->pw_name,
	(host == NULL) ? "unknown-host" : host->h_name,
	g_ntohs(local_socket.sin_port),
	program_name,
	getpid()
    );
    id[sizeof(id)-1] = '\0';

    return id;
}
#endif



static int libbdm_cal_peer_matches(const char *peer_name, const char *pattern) {

    if (strcmp(pattern, "*") == 0) return 1;
    if (strcmp(pattern, peer_name) == 0) return 1;

    return 0;
}




// 
// Opens a connection to the Bionet Data Manager network.
//
// Returns the fd if the connection is open, -1 if there's a problem.
//
int bdm_start(void) {

    // if the connection is already open we just return it's fd
    if (libbdm_cal_fd > -1) return libbdm_cal_fd;


    //
    // If we get here we need to actually open the connection.
    //


    libbdm_cal_fd = cal_client.init("bdm", libbdm_cal_callback, libbdm_cal_peer_matches);
    if (libbdm_cal_fd == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_connect(): error initializing CAL");
        return -1;
    }


#if 0

    //
    // Send our ident string to the server, it's useful for keeping
    // statistics and for debugging.
    //
    // If the client app did not specifically set the ID by calling
    // bdm_set_id(), we use the default: "user@host:port (program [pid])"
    //

    if (libbdm_client_id != NULL) {
        bdm_message_t m;
        m.type = Bionet_Message_C2N_Set_ID;
        m.body.c2n_set_id.id = libbdm_client_id;
        r = bdm_nxio_send_message(libbdm_nag_nxio, &m);

    } else {
	bdm_message_t m;
        char *id;

        id = (char *)libbdm_get_id();
        if (id == NULL) {
            // the _get_id() function will have logged an error
            libbdm_kill_nag_connection();
            return -1;
        }

        m.type = Bionet_Message_C2N_Set_ID;
        m.body.c2n_set_id.id = (char *)libbdm_get_id();
        r = bdm_nxio_send_message(libbdm_nag_nxio, &m);
    }

    // r is from the bdm_nxio_send_message()
    if (r < 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_connect_to_nag(): error sending ident string");
        libbdm_kill_nag_connection();
        return -1;
    }

    r = libbdm_read_ok_from_nag();
    if (r < 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_connect_to_nag(): error setting id: %s", bdm_get_nag_error());
        libbdm_kill_nag_connection();
        return -1;
    }


    libbdm_clear_cache();
#endif


    return libbdm_cal_fd;
}

// 
// Opens a TCP connection to the BDM server specified by
// bdm_hostname and bdm_port.
//
// if port == 0, BDM_PORT will be used
//
// Returns the socket fd if the connection is open, -1 if there's a problem.
//

void bdm_add_server(char *hostname, uint16_t port) {
    if (libbdm_cal_fd < 0) if ( bdm_start() < 0 ) return;

    if (port == 0)
    {
	port = BDM_PORT;
    }

    if (hostname == NULL) {
        hostname = "localhost";
    }


    cal_client.force_discover(hostname, hostname, port, 0);

}

// 
// Opens a TCP connection to the BDM server specified by
// bdm_hostname and bdm_port, and waits for completion
//
// if port == 0, BDM_PORT will be used
//
// Returns the socket fd if the connection is open, -1 if there's a problem.
//

int bdm_connect(char *hostname, uint16_t port) {
    int rc = 0;

    if (libbdm_cal_fd < 0) if( bdm_start() < 0 ) return -1;

    if (port == 0)
    {
	port = BDM_PORT;
    }

    if (hostname == NULL) {
        hostname = BDM_DEFAULT_HOST;
    }


    bionet_bdm_t * bdm = bionet_bdm_new(hostname);
    if ( bdm == NULL ) return -1; // Error has already been logged
    libbdm_bdm_api_subscriptions = g_slist_prepend(libbdm_bdm_api_subscriptions, bdm);

    cal_client.force_discover(hostname, hostname, port, 0);

    int found = 0;
    while(!found) {
        bdm_read();
        GSList * l;

        for(l = libbdm_api_new_peers; l != NULL;) {
            char * peer_name = l->data;
            if (!strcmp(peer_name, bdm->id)) {
                found = 1;
                rc = 0;
            }
            free(l->data);
            l = g_slist_delete_link(l, l);
        }

        for(l = libbdm_api_lost_peers; l != NULL;) {
            char * peer_name = l->data;
            if (!strcmp(peer_name, bdm->id)) {
                found = 1;
                rc = -1;
            }
            free(l->data);
            l = g_slist_delete_link(l, l);
        }
    }
    libbdm_bdm_api_subscriptions = g_slist_remove(libbdm_bdm_api_subscriptions, bdm);


    return rc;

}

