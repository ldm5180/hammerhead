
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
#include "libbdm-internal.h"


int bdm_is_connected() {
    if (bdm_fd < 0) {
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

    if (strcmp(pattern, "*") == 0) return 0;
    if (strcmp(pattern, peer_name) == 0) return 0;

    return 1;
}




// 
// Opens a connection to the Bionet Data Manager network.
//
// Returns the fd if the connection is open, -1 if there's a problem.
//
int bdm_start(void) {

    // if the connection is already open we just return it's fd
    if (libbdm_cal_handle != NULL) return cal_client.get_fd(libbdm_cal_handle);


    //
    // If we get here we need to actually open the connection, and do one-time init
    //

    libbdm_all_peers = g_hash_table_new_full(g_str_hash, g_str_equal, free, NULL);


    libbdm_cal_handle = cal_client.init("bionet-db", libbdm_cal_callback, libbdm_cal_peer_matches);
    if (libbdm_cal_handle == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_connect(): error initializing CAL");
        return -1;
    }

    return cal_client.get_fd(libbdm_cal_handle);
}


// 
// Opens a TCP connection to the BDM server specified by
// bdm_hostname and bdm_port.
//
// if port == 0, BDM_PORT will be used
//
// Returns the socket fd if the connection is open, -1 if there's a problem.
//

int bdm_connect(char *hostname, uint16_t port) {
    struct sockaddr_in server_address;
    struct hostent *server_host;


    // if the connection is already open we just return it's fd
    if (bdm_fd > -1) return bdm_fd;

    if (port == 0)
    {
	port = BDM_PORT;
    }

    if (hostname == NULL) {
        hostname = "localhost";
    }


    //
    // If the server dies or the connection is lost somehow, writes will
    // cause us to receive SIGPIPE, and the default SIGPIPE handler
    // terminates the process.  So we need to change the handler to ignore
    // the signal, unless the process has explicitly changed the action.
    //

    {
        int r;
        struct sigaction sa;

        r = sigaction(SIGPIPE, NULL, &sa);
        if (r < 0) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_connect(): error getting old SIGPIPE sigaction: %s", strerror(errno));
            return -1;
        }

        if (sa.sa_handler == SIG_DFL) {
            sa.sa_handler = SIG_IGN;
            r = sigaction(SIGPIPE, &sa, NULL);
            if (r < 0) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_connect(): error setting SIGPIPE sigaction to SIG_IGN: %s", strerror(errno));
                return -1;
            }
        }
    }


    // get the hostent for the server
    server_host = gethostbyname(hostname);

    if (server_host == NULL) {
	const char *error_string;
	error_string = hstrerror(h_errno);

        g_log(
            BDM_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bdm_connect(): gethostbyname(\"%s\"): %s",
            hostname,
            error_string
        );
        return -1;
    }


    // create the socket
    if ((bdm_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        g_log(
            BDM_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bdm_connect(): cannot create local socket: %s",
            strerror(errno)
        );
        return -1;
    }


    //  This makes it to the underlying networking code tries to send any
    //  buffered data, even after we've closed the socket.
    {
        struct linger linger;

        linger.l_onoff = 1;
        linger.l_linger = 60;
        if (setsockopt(bdm_fd, SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof(linger)) != 0) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_connect(): WARNING: cannot make socket linger: %s", strerror(errno));
        }
    }


    // prepare the server address
    memset((char *)&server_address, '\0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)(*server_host->h_addr_list)));
    server_address.sin_port = g_htons(port);


    // connect to the server
    if (connect(bdm_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        g_log(
            BDM_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bdm_connect(): failed to connect to server %s:%d: %s",
            server_host->h_name,
            port,
            strerror(errno)
        );

        bdm_disconnect();

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
#endif


    return bdm_fd;
}


