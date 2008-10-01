
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

#include <sys/socket.h>

#include <dns_sd.h>

#include "cal.h"
#include "cal-dnssd.h"


struct cal_peer_t* me;

static DNSServiceRef *advertisedRef = NULL;


#if 0
static uint16_t get_port_number(char* s) {
    char *p;
    uint16_t port;
    char *endptr;

    p = strrchr(s, ':');
    if (p == NULL) {
        // error!
        return 0;
    }

    p++;
    if (*p == '\0') {
        // error!
        return 0;
    }

    port = strtoul(p, &endptr, 0);
    if (*endptr != '\0') {
        // error!
        return 0;
    }

    return port;
}
#endif


// 
// the following functions handle the "HAB" side of CAL-PD: Joining and Leaving a network
//

static void register_callback(DNSServiceRef sdRef, 
                       DNSServiceFlags flags, 
                       DNSServiceErrorType errorCode, 
                       const char *name, 
                       const char *type, 
                       const char *domain, 
                       void *context) {
    if (errorCode != kDNSServiceErr_NoError) {
        fprintf(stderr, "dnssd: Register callback returned %d\n", errorCode);
    } else {
        // printf("dnssd: REGISTERED: %s.%s%s\n", name, type, domain);
    }
}


int dnssd_register(cal_peer_t* peer) {
    DNSServiceErrorType error;
    TXTRecordRef txt_ref;
    struct sockaddr_in *sin;


    sin = (struct sockaddr_in *)&peer->addr;


    // Shutup annoying nag message on Linux.
    setenv("AVAHI_COMPAT_NOWARN", "1", 1);


    advertisedRef = malloc(sizeof(DNSServiceRef));
    if (advertisedRef == NULL) {
        fprintf(stderr, "dnssd: out of memory!\n");
        return 0;
    }

    TXTRecordCreate(&txt_ref, 0, NULL);

#if 0
    for (i = 0; i < peer->num_unicast_addresses; i ++) {
        char key[100];

        sprintf(key, "unicast-address-%d", i);
        error = TXTRecordSetValue ( 
            &txt_ref,                          // TXTRecordRef *txtRecord, 
            key,                               // const char *key, 
            strlen(peer->unicast_address[i]),  // uint8_t valueSize, /* may be zero */
            peer->unicast_address[i]           // const void *value /* may be NULL */
        );  

        if (error != kDNSServiceErr_NoError) {
            free(advertisedRef);
            advertisedRef = NULL;
            TXTRecordDeallocate(&txt_ref);
            fprintf(stderr, "dnssd: Error registering service: %d\n", error);
            return 0;
        }
    }
#endif

    error = DNSServiceRegister(
        advertisedRef,                   // DNSServiceRef *sdRef
        0,                               // DNSServiceFlags flags
        0,                               // uint32_t interfaceIndex
        peer->name,                      // const char *name
        cal_pd_dnssd_service_name,       // const char *regtype
        "",                              // const char *domain
        NULL,                            // const char *host
        sin->sin_port,                   // uint16_t port (in network byte order)
        TXTRecordGetLength(&txt_ref),    // uint16_t txtLen
        TXTRecordGetBytesPtr(&txt_ref),  // const void *txtRecord
        register_callback,               // DNSServiceRegisterReply callBack
        NULL                             // void *context
    );

    if (error != kDNSServiceErr_NoError) {
        free(advertisedRef);
        advertisedRef = NULL;
        fprintf(stderr, "dnssd: Error registering service: %d\n", error);
        return 0;
    }

    // FIXME: the caller might want to select on the advertisedRef fd?
    return 1;
}


int dnssd_remove(cal_peer_t* peer) {
    if (advertisedRef != NULL) {
        DNSServiceRefDeallocate(*advertisedRef);
	free(advertisedRef);
	advertisedRef = NULL;
    }
    return 1;
}

