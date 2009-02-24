
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

#ifdef WINDOWS
    #include <winsock2.h>
#endif

#include <glib.h>

#include "cal-client.h"

#include "libbionet-internal.h"
#include "bionet.h"




#if 0
static void libbionet_clear_cache(void) {
    bionet_hab_t *hab;


    while ((hab = g_slist_nth_data(bionet_habs, 0)) != NULL) {
        bionet_node_t *node;

        while ((node = g_slist_nth_data(hab->nodes, 0)) != NULL) {
            if (libbionet_callback_lost_node != NULL) {
                libbionet_callback_lost_node(node);
            }

            libbionet_cache_remove_node(node);
        }

        if (libbionet_callback_lost_hab != NULL) {
            libbionet_callback_lost_hab(hab);
        }

        libbionet_cache_remove_hab(hab);
    }
}
#endif




int bionet_is_connected(void) {
    if (libbionet_cal_fd < 0) return 0;
    return 1;
}




#if 0
static const char *libbionet_get_id(void) {
#if defined(LINUX) || defined(MACOSX)
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
	    "bionet_connect_to_nag(): unable to determine user name for ident string: %s",
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
	    "bionet_connect_to_nag(): unable to gethostbyname(\"localhost\") for ident string (%s), oh well",
	    hstrerror(h_errno)
	);
    }


    // get the local socket port number
    {
	int r;
	socklen_t len;

	len = sizeof(struct sockaddr_in);
	r = getsockname(libbionet_nag_nxio->socket, (struct sockaddr *)&local_socket, &len);
	if (r < 0) {
	    g_log(
		BIONET_LOG_DOMAIN,
		G_LOG_LEVEL_WARNING,
		"bionet_connect_to_nag(): error getting local socket name: %s",
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
		"bionet_connect_to_nag(): error opening /proc/self/cmdline: %s",
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
                "bionet_connect_to_nag(): error reading /proc/self/cmdline: %s",
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
#endif

#ifdef WINDOWS

    // see http://msdn2.microsoft.com/en-us/library/ms724426.aspx
    // and http://msdn2.microsoft.com/en-us/library/ms724953.aspx

    // this one's like getpwuid
    // address of name buffer
    // address of size of name buffer
    // http://msdn2.microsoft.com/en-us/library/ms724432.aspx
//     BOOL GetUserName(
//         LPTSTR lpBuffer,
//         LPDWORD nSize
//     );

    // FIXME: do something more real here
    return "a windows user";

#endif

}
#endif




static int libbionet_cal_peer_matches(const char *peer_name, const char *pattern) {
    char peer_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char peer_id[BIONET_NAME_COMPONENT_MAX_LEN];

    char pattern_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char pattern_id[BIONET_NAME_COMPONENT_MAX_LEN];

    int r;


    r = bionet_split_hab_name_r(peer_name, peer_type, peer_id);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "libbionet_cal_peer_matches: cannot parse peer name '%s'", peer_name);
        return -1;
    }

    r = bionet_split_hab_name_r(pattern, pattern_type, pattern_id);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "libbionet_cal_peer_matches: cannot parse pattern '%s'", pattern);
        return -1;
    }

    if (!bionet_name_component_matches(peer_type, pattern_type)) return 1;
    if (!bionet_name_component_matches(peer_id, pattern_id)) return 1;

    return 0;
}




// 
// Opens a connection to the Bionet network.
//
// Returns the fd if the connection is open, -1 if there's a problem.
//

int bionet_connect(void) {

    // if the connection is already open we just return it's fd
    if (libbionet_cal_fd > -1) return libbionet_cal_fd;


    //
    // If we get here we need to actually open the connection.
    //


    libbionet_cal_fd = cal_client.init("bionet", libbionet_cal_callback, libbionet_cal_peer_matches);
    if (libbionet_cal_fd == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_connect(): error initializing CAL");
        return -1;
    }


#if 0

    //
    // Send our ident string to the server, it's useful for keeping
    // statistics and for debugging.
    //
    // If the client app did not specifically set the ID by calling
    // bionet_set_id(), we use the default: "user@host:port (program [pid])"
    //

    if (libbionet_client_id != NULL) {
        bionet_message_t m;
        m.type = Bionet_Message_C2N_Set_ID;
        m.body.c2n_set_id.id = libbionet_client_id;
        r = bionet_nxio_send_message(libbionet_nag_nxio, &m);

    } else {
	bionet_message_t m;
        char *id;

        id = (char *)libbionet_get_id();
        if (id == NULL) {
            // the _get_id() function will have logged an error
            libbionet_kill_nag_connection();
            return -1;
        }

        m.type = Bionet_Message_C2N_Set_ID;
        m.body.c2n_set_id.id = (char *)libbionet_get_id();
        r = bionet_nxio_send_message(libbionet_nag_nxio, &m);
    }

    // r is from the bionet_nxio_send_message()
    if (r < 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_connect_to_nag(): error sending ident string");
        libbionet_kill_nag_connection();
        return -1;
    }

    r = libbionet_read_ok_from_nag();
    if (r < 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_connect_to_nag(): error setting id: %s", bionet_get_nag_error());
        libbionet_kill_nag_connection();
        return -1;
    }


    libbionet_clear_cache();
#endif


    return libbionet_cal_fd;
}


