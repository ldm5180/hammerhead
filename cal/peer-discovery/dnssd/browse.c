
//
//  the following functions handle the "browse" or "Client" side
//  of CAL-PD mDNS/SD: looking for peers on a network
//


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <glib.h>

#include <dns_sd.h>

#include "cal.h"
#include "cal-dnssd.h"


// 
// this is a linked list, each payload is a (struct callback_context *)
// we add the first one when we start the browse running, then we add one each time we start a resolve
// when the resolve finishes we remove its node from the list
//

struct service_context {
    DNSServiceRef service_ref;
    cal_event_t *event;
};

GSList *service_list = NULL;




void resolve_callback(DNSServiceRef service_ref,
		      DNSServiceFlags flags,
		      uint32_t interfaceIndex,
		      DNSServiceErrorType errorCode,
		      const char *fullname,
		      const char *hosttarget,
		      uint16_t port,
		      uint16_t txtLen,
		      const unsigned char *txtRecord,
		      void *context) {

    int r;
    struct service_context *sc = context;
    cal_event_t *event = sc->event;

    // printf("dnssd: in resolve_callback\n");

    // remove this service_ref from the list
    DNSServiceRefDeallocate(sc->service_ref);
    service_list = g_slist_remove(service_list, sc);
    free(sc);

    if (errorCode != kDNSServiceErr_NoError) {
        fprintf(stderr, "dnssd: Error returned from resolve: %d\n.", errorCode);
        return;
    }

    port = ntohs(port);

#if 0
    printf(
        "dnssd: resolved fullname='%s', hosttarget='%s', port=%d, txt='%s'\n",
        fullname,
        hosttarget,
        port,
        txtRecord
    );
#endif


    // parse all the unicast addresses out of the txt record
    {
        int i;

        i = 0;
        do {
            char key[100];
            const char *value;
            uint8_t valueLen;

            sprintf(key, "unicast-address-%d", i);

            value = TXTRecordGetValuePtr(txtLen, txtRecord, key, &valueLen);
            if (value == NULL) {
                // printf("    no more unicast addresses\n");
                break;
            }
            // printf("    unicast address %d: '%s'\n", i, value);

            if (cal_peer_add_unicast_address(&event->peer, value) != 0) {
                printf("error adding unicast address '%s' to peer '%s'\n", value, event->peer.name);
                break;
            }

            i ++;
        } while (1);
    }


    // the event becomes the responsibility of the callback now, so they might leak memory but we're not
    r = write(cal_pd_dnssd_browser_fds[1], &event, sizeof(event));  // heh
    if (r < 0) {
        printf("dnssd: error writing event: %s\n", strerror(errno));
    } else if (r != sizeof(event)) {
        printf("dnssd: short write while writing event\n");
    }
} 


void browse_callback(DNSServiceRef service,
		     DNSServiceFlags flags,
		     uint32_t interfaceIndex,
		     DNSServiceErrorType errorCode,
		     const char *name,
		     const char *type,
		     const char *domain,
		     void *context) {

    cal_event_t *event;


#if 0
    printf(
        "dnssd: in browse_callback: interfaceIndex=%d, errorCode=%d, name='%s', type='%s', domain='%s'\n",
        interfaceIndex,
        errorCode,
        name,
        type,
        domain
    );
#endif

    if (errorCode != kDNSServiceErr_NoError) {
        fprintf(stderr, "dnssd: Error returned from browse: %d\n", errorCode);
        return;
    }


    event = calloc(1, sizeof(cal_event_t));
    if (event == NULL) {
        fprintf(stderr, "dnssd: out of memory!  dropping this joining peer!\n");
        return;
    }
    event->peer.name = strdup(name);
    if (event->peer.name == NULL) {
        fprintf(stderr, "dnssd: out of memory!  dropping this joining peer!\n");
        free(event);
        return;
    }

    if (flags & kDNSServiceFlagsAdd) {
        struct service_context *sc;
        DNSServiceErrorType error;

        // printf("dnssd: trying to resolve...\n");

        sc = malloc(sizeof(struct service_context));
        if (sc == NULL) {
            fprintf(stderr, "dnssd: out of memory!  dropping this joining peer!\n");
            return;
        }

        sc->event = event;
        sc->event->event_type = CAL_EVENT_JOIN;

        // Now create a resolve call to fill out the rest of the cal_peer_t
        error = DNSServiceResolve(&sc->service_ref, 0, interfaceIndex, 
                                  name, type, domain, 
                                  resolve_callback, (void*)sc);
        if (error != kDNSServiceErr_NoError) {
            fprintf(stderr, "dnssd: failed to start resolv service, dropping this peer\n");
            free(sc->event->peer.name);
            free(sc->event);
            free(sc);
            return;
        }

        // add this new service ref to the list
        // printf("adding this resolve from service ref list\n");
        service_list = g_slist_prepend(service_list, sc);
    } else {
        int r;

        event->event_type = CAL_EVENT_LEAVE;

        // Do we really need to fillout all the fields in the cal_peer_t?
        // Probably not.  Plus it is now gone.  For now, just fill out
        // the name.

        // the event becomes the responsibility of the callback now, so they might leak memory but we're not
        r = write(cal_pd_dnssd_browser_fds[1], &event, sizeof(event));  // heh
        if (r < 0) {
            printf("dnssd: error writing event: %s\n", strerror(errno));
        } else if (r != sizeof(event)) {
            printf("dnssd: short write while writing event\n");
        }
    }
}


void cal_pd_dnssd_end_browse(void) {
    GSList *ptr;

    ptr = service_list;
    while (ptr != NULL) {
        GSList *next = ptr->next;
        struct service_context *sc = ptr->data;
        DNSServiceRefDeallocate(sc->service_ref);
        service_list = g_slist_remove(service_list, sc);
	free(sc);
        ptr = next;
    }
}


//
// this function gets run by the thread started by subscribe_peer_list()
// canceled by unsubscribe_peer_list()
//

void *cal_pd_dnssd_browser_function(void *arg) {
    struct service_context *browse;
    DNSServiceErrorType error;

    // Shutup annoying nag message on Linux.
    setenv("AVAHI_COMPAT_NOWARN", "1", 1);

    browse = malloc(sizeof(struct service_context));
    if (browse == NULL) {
        fprintf(stderr, "dnssd: out of memory!\n");
        return NULL;
    }

    error = DNSServiceBrowse(&browse->service_ref, 0, 0,
			     cal_pd_dnssd_service_name, NULL,  
			     browse_callback, NULL);

    if (error != kDNSServiceErr_NoError) {
        free(browse);
	fprintf(stderr, "dnssd: Error browsing for service: %d\n", error);
	return NULL;
    }

    service_list = g_slist_prepend(service_list, browse);
    

    // main loop: select on all our open service references, and handle any events
    while (1) {
        GSList *ptr;
        fd_set readers;
        int max_fd;
        int r;

        FD_ZERO(&readers);
        max_fd = -1;

        r = 0;
        for (ptr = service_list; ptr != NULL; ptr = ptr->next) {
            struct service_context *sc = ptr->data;
            int fd = DNSServiceRefSockFD(sc->service_ref);
            r ++;
            FD_SET(fd, &readers);
            max_fd = Max(max_fd, fd);
        }
        // printf("%d service refs in list\n", r);

        // block until there's something to do
        // printf("dnssd: entering select\n");
        r = select(max_fd + 1, &readers, NULL, NULL, NULL);
        // printf("dnssd: select returns\n");

        for (ptr = service_list; ptr != NULL; ptr = ptr->next) {
            struct service_context *sc = ptr->data;
            int fd = DNSServiceRefSockFD(sc->service_ref);
            if (FD_ISSET(fd, &readers)) {
                DNSServiceErrorType err;

                err = DNSServiceProcessResult(sc->service_ref);
                if (err != kDNSServiceErr_NoError) {
                    fprintf(stderr, "dnssd: Error processing service reference result: %d.\n", err);
                    // FIXME
                    return NULL;
                }
                break;
            }
        }
    }
}

