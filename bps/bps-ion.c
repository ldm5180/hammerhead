
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include "bps_socket.h"

#include "bps-ion-private.h"
#include "bps-sock-table.h"


#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <pthread.h>

/*
 * All bps library functions for ion are implemented in this file
 *
 */

// Group ion configs for clarity
typedef struct {
    Sdr	sdr; // The SDR handle
} ion_config_t;

static struct {
    long basekey;
} opts;

ion_config_t ion;

static int bp_attached = 0;




/**************************************************
 * API functions
 *************************************************/

// Make sure all ION libraries are loaded, and setup
int bps_socket(int domain, int type, int protocol) {
    bps_socket_t * sock = NULL;


    // Initialize ION, if needed...
    if (!bp_attached) {
        if( load_ion() != 0) return -1;

        if ((*bdm_bp_funcs.bp_attach)() < 0) return -1;
        bp_attached++;

        ion.sdr = (*bdm_bp_funcs.bp_get_sdr)();
    }

    // Create new connection object
    sock = bps_sock_new();
    if ( sock == NULL ) return -1;

    return sock->usrfd;
}

int bps_setsockopt(int sockfd, int level, int optname, 
        const void *optval, socklen_t optlen)
{
    bps_socket_t * sock = bps_sock_table_lookup_fd(sockfd);
    if ( NULL == sock ) {
        errno = EBADF;
        return -1;
    }

    if ( level != SOL_BP 
    ||   NULL == optval )
    {
        errno = EINVAL;
        return -1;
    }


    int ival;
    switch ( optname ) {
        case BPS_SO_BDL_LIFETIME:
            if ( optlen != sizeof(int) 
            ||   *(int*)optval <= 0 )
                errno = EINVAL; return -1;

            sock->opts.bundleLifetime = *(int*)optval;
            break;

        case BPS_SO_BDL_PRIORITY:
            if ( optlen != sizeof(int) )
                errno = EINVAL; return -1;

            ival = *(int*)optval;
            if ( ival < 0 || ival > 2 )
                errno = EINVAL; return -1;

            sock->opts.classOfService = ival;
            break;

        case BPS_SO_REQ_CUSTODY:
            if ( optlen != sizeof(int) )
                errno = EINVAL; return -1;

            ival = *(int*)optval;
            if ( ival < 0 || ival > 2 )
                errno = EINVAL; return -1;

            sock->opts.custodySwitch = ival;
            break;

        default:
            errno = ENOPROTOOPT;
            return -1;
    }

    return 0;
}

#if 0
int bps_getsockopt(int sockfd, int level, int optname, 
        void *optval, socklen_t *optlen)
{
    int r;

    bps_socket_t * sock = bps_sock_table_lookup_fd(sockfd);
    if ( NULL == sock ) {
        errno = EBADF;
        return -1;
    }

}
#endif

int bps_bind(int sockfd, struct bps_sockaddr *addr, socklen_t addrlen)
{
    int r;

    bps_socket_t * sock = bps_sock_table_lookup_fd(sockfd);
    if ( NULL == sock ) {
        errno = EBADF;
        return -1;
    }

    if ( sock->have_local_uri ) {
        errno = EINVAL;
        return -1;
    }

    strncpy(sock->local_uri, addr->uri, BPS_EID_SIZE);
    sock->have_local_uri = 1;

    r = (*bdm_bp_funcs.bp_open)(sock->local_uri, &sock->sap);
    if (r < 0) {
        if (bdm_bp_funcs.bp_add_endpoint != NULL) {
            if ((*bdm_bp_funcs.bp_add_endpoint)(sock->local_uri, NULL) != 0) {
                errno = EADDRNOTAVAIL;
                return -1;
            }
            r = (*bdm_bp_funcs.bp_open)(sock->local_uri, &sock->sap);
        }
        if (r < 0) {
            errno = EADDRNOTAVAIL;
            return -1;
        }
    }

    // Start ION threads
    sock->recv_thread = start_ion_recv_thread(sock->libfd, sock->sap);
    if ( sock->recv_thread  == NULL ) return -1;

    return 0;
}


int bps_connect(int sockfd, struct bps_sockaddr *addr, socklen_t addrlen)
{
    bps_socket_t * sock = bps_sock_table_lookup_fd(sockfd);
    if ( NULL == sock ) {
        errno = EBADF;
        return -1;
    }

    if ( sock->have_remote_uri ) {
        errno = EISCONN;
        return -1;
    }

    strncpy(sock->remote_uri, addr->uri, BPS_EID_SIZE);
    sock->have_remote_uri = 1;

    return 0;
}

int bps_listen(int sockfd, int backlog)
{
    bps_socket_t * sock = bps_sock_table_lookup_fd(sockfd);
    if ( NULL == sock ) {
        errno = EBADF;
        return -1;
    }

    if ( ! sock->have_local_uri ) {
        // The socket needs to be bound first
        errno = EBADFD;
    }

    sock->listening = 1;

    return 0;
}

///// Helpers for receiving
static int _bdl_open(BpDelivery *dlv, ZcoReader *reader)
{

        (*bdm_bp_funcs.sdr_begin_xn)(ion.sdr);
        (*bdm_bp_funcs.zco_start_receiving)(ion.sdr, dlv->adu, reader);
        
        if ((*bdm_bp_funcs.sdr_end_xn)(ion.sdr) < 0)
        {
            errno = EPROTO;
            return -1;
        }

        return 0;
}

static int _bdl_close(BpDelivery *dlv, ZcoReader *reader)
{

        (*bdm_bp_funcs.sdr_begin_xn)(ion.sdr);

        (*bdm_bp_funcs.zco_stop_receiving)(ion.sdr, reader);

        if ((*bdm_bp_funcs.sdr_end_xn)(ion.sdr) < 0)
        {
            errno = EPROTO;
            return -1;
        }

        return 0;
}

int bps_accept(int sockfd, struct bps_sockaddr *addr, socklen_t *addrlen)
{
    int r;

    bps_socket_t * sock = bps_sock_table_lookup_fd(sockfd);
    if ( NULL == sock ) {
        errno = EBADF;
        return -1;
    }

    if( !sock->listening  ) {
        // The socket needs to be bound first
        errno = EBADFD;
        return -1;
    }

    // Create new accept socket
    bps_socket_t * accept_sock = NULL;
    accept_sock = bps_sock_new();
    if ( NULL == accept_sock ) return -1;


    // Get the bundle from ION RECV THREAD
    BpDelivery *dlv;
    r = pop_recv_bundle(sockfd, &dlv);
    if ( r <= 0 ) {
        if ( r == 0 ) {
            errno = EPIPE;
        }
        return -1;
    }

    // Setup the accept socket with bundle
    if(sock->have_local_uri) {
        strcpy(accept_sock->local_uri, sock->local_uri);
        accept_sock->have_local_uri = 1;
    }

    strncpy(accept_sock->remote_uri, dlv->bundleSourceEid, BPS_EID_SIZE);
    accept_sock->have_remote_uri = 1;

    accept_sock->bundle = dlv;

    accept_sock->accept.total_len = (*bdm_bp_funcs.zco_source_data_length)(ion.sdr, accept_sock->bundle->adu);

    r = _bdl_open(accept_sock->bundle, &accept_sock->accept.reader);
    if ( r ) return -1;

    // Tell user to read from accept socket
    char buf[1] = {0};
    ssize_t bytes;
    while((bytes = write(accept_sock->libfd, buf, sizeof(buf))) < 0 && errno == EINTR);
    if(bytes != sizeof(buf)){
        return -1;
    }

    return accept_sock->usrfd;
}


static ssize_t _bdl_read(BpDelivery *dlv, ZcoReader *reader, void * buf, size_t len)
{

        int bytes;

        (*bdm_bp_funcs.sdr_begin_xn)(ion.sdr);
        
        bytes = (*bdm_bp_funcs.zco_receive_source)(ion.sdr, reader,
                        len, buf);

        if ((*bdm_bp_funcs.sdr_end_xn)(ion.sdr) < 0)
        {
            errno = EPROTO;
            return -1;
        }

        return bytes;
}

int bps_recv(int sockfd, void *buf, size_t len, int flags)
{
    return bps_recvfrom(sockfd, buf, len, flags, NULL, 0);
}

int bps_recvfrom(int sockfd, void *buf, size_t len, int flags, 
        struct bps_sockaddr *src_addr, socklen_t *addrlen)
{
    int r; 

    bps_socket_t * sock = bps_sock_table_lookup_fd(sockfd);
    if ( NULL == sock ) {
        errno = EBADF;
        return -1;
    }

    if ( !sock->have_local_uri ) {
        // Can't recieve if not bound
        errno = EINVAL;
        return -1;
    }

    if ( sock->bundle) {
        // This is an accept socket
        //
        // The bundle has been opened already...
        int bytes = _bdl_read(sock->bundle, &sock->accept.reader, buf, len);
        if ( bytes < 0 ) return -1;

        return bytes;
    } else {
        // Not an accept socket
        ZcoReader reader;

        BpDelivery *dlv;
        r = pop_recv_bundle(sockfd, &dlv);
        if( r <= 0 ) {
            return -1;
        }

        unsigned int actual_len = (*bdm_bp_funcs.zco_source_data_length)(ion.sdr, dlv->adu);

        // Get Source addr, if user wants it
        if ( src_addr && *addrlen == sizeof(struct bps_sockaddr) ) {
            strncpy(src_addr->uri, dlv->bundleSourceEid, BPS_EID_SIZE);
            src_addr->uri[BPS_EID_SIZE-1] = '\0';
        }


        (*bdm_bp_funcs.sdr_begin_xn)(ion.sdr);
        (*bdm_bp_funcs.zco_start_receiving)(ion.sdr, dlv->adu, &reader);
        
        ssize_t bytes = (*bdm_bp_funcs.zco_receive_source)(ion.sdr, &reader,
                        len, buf);

        (*bdm_bp_funcs.zco_stop_receiving)(ion.sdr, &reader);
        if ((*bdm_bp_funcs.sdr_end_xn)(ion.sdr) < 0)
        {
            errno = EPROTO;
            return -1;
        }

        if(bytes < actual_len && bytes < len)
        {
            // Not enough bytes were copied :(
            errno = EPROTO;
            return -1;
        }

        if(flags & MSG_TRUNC) {
            return actual_len;
        }

        return bytes;
    }

}

int bps_send(int sockfd, void *buf, size_t len, int flags)
{
    return bps_sendto(sockfd, buf, len, flags, NULL, 0);
}

int bps_sendto(int sockfd, void *buf, size_t len, int flags, 
        struct bps_sockaddr *dst_addr, socklen_t addrlen)
{

    bps_socket_t * sock = bps_sock_table_lookup_fd(sockfd);
    if ( NULL == sock ) {
        errno = EBADF;
        return -1;
    }

    if ( dst_addr == NULL && ! sock->have_remote_uri ) {
        // Need a dest addr
        errno = EDESTADDRREQ;
        return -1;
    }

    if(sock->send_thread == NULL) {
        sock->send_thread = start_ion_send_thread(sock->libfd, sock->sap);
        if ( sock->send_thread  == NULL ) return -1;
    }

    if (buf && len) {
        Object extent;
        /*
        (*bdm_bp_funcs.sdr_begin_xn)(ion.sdr);
        (*bdm_bp_funcs.zco_start_receiving)(ion.sdr, dlv->adu, reader);
        
        if ((*bdm_bp_funcs.sdr_end_xn)(ion.sdr) < 0)
        {
            errno = EPROTO;
            return -1;
        }
        */

        (*bdm_bp_funcs.sdr_begin_xn)(ion.sdr);

        if(0 == sock->send.bundleZco) {
            sock->send.bundleZco = (*bdm_bp_funcs.zco_create)(ion.sdr, ZcoSdrSource, 0, 0, 0);
        }

        extent = bdm_sdr_malloc(ion.sdr, len);
        if (extent == 0)
        {
            (*bdm_bp_funcs.sdr_cancel_xn)(ion.sdr);
            ion.sdr = NULL;

            errno = ENOMEM;
            return -1;
        }

        bdm_sdr_write(ion.sdr, extent, buf, len);

        (*bdm_bp_funcs.zco_append_extent)(ion.sdr, 
                sock->send.bundleZco, ZcoSdrSource, extent, 0, len);

        sock->send.bundle_size += len;

        if ((*bdm_bp_funcs.sdr_end_xn)(ion.sdr) < 0)
        {
            errno = ENOMEM;
            return -1;
        }
    }


    if ( 0 == (flags & MSG_MORE) && sock->send.bundle_size ) {
        int r = push_send_bundle(sockfd, sock->send.bundleZco, 
                dst_addr?dst_addr->uri:sock->remote_uri,
                sock->opts.bundleLifetime,
                sock->opts.classOfService,
                sock->opts.custodySwitch);
        sock->send.bundleZco = 0;

        if( r ) {
            close(sockfd);
            errno = EPIPE;
            return -1;
        }
    }

    return len;
}

int bps_close(int sockfd)
{
    int r = 0;
    int errno_save = errno;
    bps_socket_t * sock = bps_sock_table_lookup_fd(sockfd);
    if ( NULL == sock ) {
        errno = EBADF;
        return -1;
    }

    if ( sock->bundle ) {
        // An accept sock
        r = _bdl_close(sock->bundle, &sock->accept.reader);
	if (r) {
	    errno_save = errno;
	}
        sock->bundle = NULL;
    }


    // 1. Remove sock from table
    bps_sock_table_remove_fd(sockfd);

    // 2. Close socket to threads, so they know to shutdown
    close(sockfd);

    // 3. Stop and wait for threads
    if(sock->recv_thread) {
        stop_ion_recv_thread(sock->recv_thread);
    }
    if(sock->send_thread) {
        stop_ion_send_thread(sock->send_thread);
    }

    // 5. Free sock
    if(sock->have_local_uri){
        (*bdm_bp_funcs.bp_close)(sock->sap);
    }

    free(sock);
    
    if (r) {
	errno = errno_save;
    }

    return r;
}

void bps_destroy(void) {
    // TODO: Verify all fds are closed, so ION threads will exit
}

int bps_setopt(int impl_type, int opt, void* optval, size_t optlen)
{
    if(impl_type != BPST_ION) {
        errno = ENOPROTOOPT;
        return -1;
    }

    switch(opt) {
        case BPSO_ION_BASEKEY:
        {
            if(optlen != sizeof(long)) {
                errno = EINVAL;
                return -1;
            }
            opts.basekey = *(long*)optval;

            if (load_ion() != 0) {
                return -1;
            }

            if(bdm_bp_funcs.sm_set_basekey) {
                (*bdm_bp_funcs.sm_set_basekey)(opts.basekey);
            } else {
                errno = ENOPROTOOPT;
                return -1;
            }
            break;

        }


        default:
            errno = ENOPROTOOPT;
            return -1;
    }

    return 0;
}

