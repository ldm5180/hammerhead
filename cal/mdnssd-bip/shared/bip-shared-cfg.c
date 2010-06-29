
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "cal-mdnssd-bip.h"

#include <stdlib.h>
#include <glib.h>
#include <pthread.h>

// POSIX
#ifdef HAVE_EMBEDDED_MDNSSD
mDNS mDNSStorage;
mDNS_PlatformSupport *PlatformSupportStorage;
CacheEntity *rrcachestorage;
int mDNS_instances = 0;

// grow the cache as necessary
void mDNS_StatusCallback(mDNS *const m, mStatus status) {
    if (status == mStatus_GrowCache) {
        CacheEntity *storage = calloc(RR_CACHE_SIZE, sizeof(CacheEntity));
        if (storage) {
            mDNS_GrowCache(m, storage, RR_CACHE_SIZE);
        } else {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "mDNS_StatusCallback: out of memory!");
        }
    }
}

int cal_mDNS_init(mDNS *m, struct timeval **timeout) {
    mStatus status = mStatus_NoError;

    if ( !mDNS_instances ) {
        rrcachestorage = calloc(RR_CACHE_SIZE, sizeof(CacheEntity));
        if (rrcachestorage == NULL) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "cal_mDNS_init: out of memory!");
            return 0;
        }

        PlatformSupportStorage = calloc(1, sizeof(mDNS_PlatformSupport));
        if (PlatformSupportStorage == NULL) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "cal_mDNS_init: out of memory!");
            return 0;
        }

        memset(m, '\x00', sizeof(mDNS));

        status = mDNS_Init(
            m,
            PlatformSupportStorage,
            rrcachestorage,
            RR_CACHE_SIZE,
            mDNS_Init_AdvertiseLocalAddresses,
            mDNS_StatusCallback,
            mDNS_Init_NoInitCallbackContext
        );
    }

    if (status != mStatus_NoError) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "cal_mDNS_init: Error initializing mDNS %ld", (long)status);
        return 0;
    }

    *timeout = calloc(1, sizeof(struct timeval));
    if (*timeout == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "cal_mDNS_init: out of memory!");
        return 0;
    }

    // increment threads using mDNS
    mDNS_instances++;

    return 1;
}


// cleanup
void mDNS_Terminate() {
    mDNS_instances--;

    if (mDNS_instances <= 0) {
        mDNS_Close(&mDNSStorage);

        free(PlatformSupportStorage);
        free(rrcachestorage);
    }
}
#endif

bip_shared_config_t bip_shared_cfg;
pthread_mutex_t avahi_mutex = PTHREAD_MUTEX_INITIALIZER;

void bip_shared_config_init(void) {

    bip_shared_cfg.max_write_buf_size = BIP_MAX_WRITE_BUF_SIZE;

    char * str = getenv("BIP_WRITE_BUFSIZE");
    if(str) {
        char * endptr;
        long x = strtol(str, &endptr, 10);

        if(x > 0 && endptr > str && *endptr == '\0') {
            bip_shared_cfg.max_write_buf_size = x;
            //g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_INFO, 
            //        "Setting BIP_WRITE_BUFSIZE to %ld", x);
        }
    }
}


