
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>

#include <glib.h>
#include <errno.h>

#include "config.h"


#ifdef ENABLE_ION
#include <zco.h>
#include <sdr.h>
#include <bp.h>

// Group ion configs for clarity
typedef struct {
    int basekey;          // ION base key, to allow multiple instance on a machine
    BpSAP sap;
} ion_config_t;

static ion_config_t ion_config;
#endif /* ENABLE_ION */

#define LOG_DOMAIN ((char *)NULL)


int bionet_bp_start(char *source_eid) {
#ifdef ENABLE_ION
    int r;

    r = bp_attach();
    if (r != 0) {
        g_log(LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bp_start(): error attaching to ION!");
        return -1;
    }

#ifdef HAVE_BP_ADD_ENDPOINT
    r = bp_open(source_eid, &ion_config.sap);
    if (r < 0) {
	if (bp_add_endpoint(source_eid, NULL) != 1) {
	    g_log(LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bp_start(): can't create source endpoint ('%s')", source_eid);
            goto fail;
	}
        r = bp_open(source_eid, &ion_config.sap);
        if (r < 0) {
	    g_log(LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bp_start(): can't open source endpoint ('%s')", source_eid);
            goto fail;
        }
    }
#else // *not* HAVE_BP_ADD_ENDPOINT
    r = bp_open(source_eid, &ion_config.sap);
    if (r < 0) {
        g_log(LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bp_start(): can't open source endpoint ('%s')", source_eid);
        goto fail;
    }
#endif // HAVE_BP_ADD_ENDPOINT

    writeErrmsgMemos();

    return 0;

fail:
    bp_detach();
    return -1;
#else // ENABLE_ION
    errno = ENOSYS;
    return -1;
#endif // ENABLE_ION
}




void bionet_bp_stop(void) {
#ifdef ENABLE_ION
    bp_detach();
#endif // ENABLE_ION
}




int bionet_bp_set_resource_by_name(char *dest_eid, int bundle_lifetime_seconds, char *resource_name, char *value) {
#ifdef ENABLE_ION
    int bundle_size;
    Sdr sdr;
    Object bundle_payload;
    Object bundle_zco;
    int offset;

    int r;
    Object unused_new_bundle;  // ION needs this...


    if (dest_eid == NULL) {
        g_log(LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bp_set_resource_by_name(): NULL destination EID passed in!");
        return -1;
    }

    if (resource_name == NULL) {
        g_log(LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bp_set_resource_by_name(): NULL Resource name passed in!");
        return -1;
    }

    if (value == NULL) {
        g_log(LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bp_set_resource_by_name(): NULL value string passed in!");
        return -1;
    }

    if (ion_config.sap == 0) {
        g_log(LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bp_set_resource_by_name(): ION is not initialized, call bionet_bp_start() first!");
        return -1;
    }


    //
    // make the bundle
    //

    bundle_size = strlen(resource_name) + 1;
    bundle_size += strlen(value) + 1;

    sdr = bp_get_sdr();

    sdr_begin_xn(sdr);

    bundle_payload = sdr_malloc(sdr, bundle_size);
    if (bundle_payload == 0) {
        g_log(LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bp_set_resource_by_name(): no space for bundle in ION SDR!");
        sdr_cancel_xn(sdr);
        return -1;
    }

    sdr_write(sdr, bundle_payload, resource_name, strlen(resource_name)+1);
    offset = strlen(resource_name) + 1;

    sdr_write(sdr, bundle_payload + offset, value, strlen(value)+1);

    bundle_zco = zco_create(sdr, ZcoSdrSource, bundle_payload, 0, bundle_size);
    if (bundle_zco == 0) {
        g_log(LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bp_set_resource_by_name(): error making ION ZCO!");
        sdr_cancel_xn(sdr);
        return -1;
    }

    if (sdr_end_xn(sdr) < 0) {
        g_log(LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bp_set_resource_by_name(): error ending ION SDR transaction!");
        sdr_cancel_xn(sdr);
        return -1;
    }


    r = bp_send(
        ion_config.sap,
        BP_BLOCKING,
        dest_eid,
        NULL,                     // report-to EID
        bundle_lifetime_seconds,  // Lifetime in seconds
        BP_STD_PRIORITY,          // class of service
        SourceCustodyRequired,
        0,                        // reporting flags - all disabled
        0,                        // app-level ack requested - what's this doing in BP?!
        NULL,                     // extended CoS - not used when CoS is STD_PRIORITY as above
        bundle_zco,
        &unused_new_bundle        // handle to the bundle in the BA, we dont need it (wish we could pass in NULL here)
    );

    if (r < 1) {
        // FIXME: what's the right way to delete the bundle here?
        g_log(LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bp_set_resource_by_name(): error sending ION bundle!");
        zco_destroy_reference(sdr, bundle_zco);
        return -1;
    }

    writeErrmsgMemos();
    return 0;
#else // ENABLE_ION
    errno = ENOSYS;
    return -1;
#endif // ENABLE_ION
}

