
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


// 
// The functions in this file take strings containing the names of various
// Bionet objects and split the strings into the components of the names.
//

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"

int bionet_param_to_timeval(GHashTable * params, const char * key, struct timeval * tv) {

    if ( params == NULL ) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): NULL params passed in", __FUNCTION__);
        return -1;
    }

    const char * valstr = (const char *)g_hash_table_lookup(params, key);
    if ( valstr ) {
        long lsec = 0, lusec=0;
        char * endptr;
        lsec = strtol(valstr, &endptr, 10);
        if ( endptr > valstr && endptr[0] == '.' ) {
            lusec = strtol(endptr+1, &endptr, 10);
        }
        if (endptr[0] != '\0' ) {
            // Conversion found bad bytes
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "%s(): Bad param (%s=%s)", __FUNCTION__, 
                    key, valstr);
            return -1;
        }
        tv->tv_sec = lsec;
        tv->tv_usec = lusec;
        return 0;
    }

    return -1;
}

#if 0
long bionet_param_to_long(GHashTable * params, const char * key) {
    long lval = -1;

    const char * valstr = (const char *)g_hash_table_lookup(params, key);

    if ( valstr ) {
        char * endptr;
        lval = strtol(valstr, &endptr, 10);
        if (*endptr != '\0' ) {
            // Conversion found bad bytes
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "%s(): Bad param (%s=%s)", __FUNCTION__, 
                    key, valstr);
            return -1;
        }
    }

    return lval;
}
#endif


int bionet_param_from_timeval(GHashTable * params, const char * key, const struct timeval * tv) {
    char * valstr = malloc(20);
    if(valstr == NULL){
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): Out of memory", __FUNCTION__);
        return -1;
    }
    snprintf(valstr, 20, "%ld.%06ld", (long)tv->tv_sec, (long)tv->tv_usec);
    g_hash_table_insert(params, strdup(key), valstr);
    return 0;
}

//
// Parse out the specific params from the query string.
//
// Will find the querystring starting at '?'. 
// If '?' missing, looks at start of querysting
//
// @param ret_params
//   GHashTable pointer reference that will return a newly allocated GHashTable
//   on success. This must be destroyed be caller with g_hash_table_destroy()
//
// Returns 0 on success, -1 on failure
//
int bionet_parse_topic_params(
        const char * querystring,
        GHashTable ** ret_params)
{
    const char *p;
    char *separator;
    int size = 0;


    GHashTable * params = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    if ( params == NULL ) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s: Out of memory!", __FUNCTION__);
        return -1;
    }

    if( (p = strchr(querystring, '?')) ) {
        querystring = p + 1;
    }

    if (strlen(querystring) > 0) {
        const char * next_p = NULL;
        for(p = querystring; p && p[0] != '\0'; p=next_p) {
            int key_size, val_size;
            char *key, *value;
            separator = strchr(p, '&');
            if (separator != NULL) {
                size = separator - p;
                next_p = separator+1;
            } else {
                size = strlen(p);
                next_p = NULL;
            }

            separator = memchr(p, '=', size);
            if (separator == NULL) {
                g_hash_table_destroy(params);
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                        "%s(): Bad topic param format '%s'", 
                        __FUNCTION__, querystring);
                return -1;
            }

            key_size = separator - p;
            val_size = size - (key_size + 1);

            key = malloc(key_size + 1);
            if ( key == NULL ) {
                g_hash_table_destroy(params);
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s: Out of memory!", __FUNCTION__);
                return -1;
            }
            memcpy(key, p, key_size);
            key[key_size] = '\0';

            value = malloc(val_size + 1);
            if ( value == NULL ) {
                g_hash_table_destroy(params);
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s: Out of memory!", __FUNCTION__);
		free(key);
                return -1;
            }
            memcpy(value, separator + 1, val_size);
            value[val_size] = '\0';

            g_hash_table_insert(params, key, value);

        }
    }

    *ret_params = params;

    return 0;
}

int bdm_split_bdm_peer_id(
        const char * name,
        char peer_id[BIONET_NAME_COMPONENT_MAX_LEN],
        char bdm_id[BIONET_NAME_COMPONENT_MAX_LEN] )
{
    const char * peer_separator = strchr(name, ',');
    int size;

    const char * p = name;

    if (peer_separator == NULL) {
        // No Peer means all peers
        if(peer_id) strncpy(peer_id, "*", BIONET_NAME_COMPONENT_MAX_LEN);
        size = strlen(name);
    }else {
        int peer_size = peer_separator - name;

        if (peer_size > BIONET_NAME_COMPONENT_MAX_LEN-1) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "%s(): Peer-ID of Name '%s' is too long (%d bytes, max %lu)", 
                    __FUNCTION__, name, peer_size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN-1);
            return -1;
        }

        if(peer_id) {
            memcpy(peer_id, name, peer_size);
            peer_id[peer_size] = '\0';
        }

        size = strlen(peer_separator+1);
        p = peer_separator + 1;
    }

    if (size > BIONET_NAME_COMPONENT_MAX_LEN-1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): BDM-ID of Topic '%s' is too long (%d bytes, max %lu)", 
                __FUNCTION__, name, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN-1);
        return -1;
    }

    if(bdm_id) {
        memcpy(bdm_id, p, size);
        bdm_id[size] = '\0';
    }

    return 0;
}



static const char * _remove_bdm_prefix(
        const char * name,
        char peer_id[BIONET_NAME_COMPONENT_MAX_LEN],
        char bdm_id[BIONET_NAME_COMPONENT_MAX_LEN] )
{
    const char *p;
    char *separator;
    char *peer_separator;

    //
    // sanity checks
    //

    if (name == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): NULL Topic passed in");
        return NULL;
    }


    p = name;

    // get the BDM-ID (If specified)
    separator = strchr(p, '/');
    if (separator == NULL) {
        // No BDM means all bdms
        if(peer_id) strncpy(peer_id, "*", BIONET_NAME_COMPONENT_MAX_LEN);
        if(bdm_id) strncpy(bdm_id, "*", BIONET_NAME_COMPONENT_MAX_LEN);
    } else {
        int size;
        size = separator - p;

        peer_separator = memchr(p, ',', size);
        if (peer_separator == NULL) {
            // No Peer means all peers
            if(peer_id) strncpy(peer_id, "*", BIONET_NAME_COMPONENT_MAX_LEN);
        }else {
            int peer_size = peer_separator - p;

            if (peer_size > BIONET_NAME_COMPONENT_MAX_LEN-1) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                        "%s(): Peer-ID of Topic '%s' is too long (%d bytes, max %lu)", 
                        __FUNCTION__, name, peer_size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN-1);
                return NULL;
            }

            if(peer_id) {
                memcpy(peer_id, name, peer_size);
                peer_id[peer_size] = '\0';
            }

            p = peer_separator + 1;
            size = separator - p;
        }

        if (size > BIONET_NAME_COMPONENT_MAX_LEN-1) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "%s(): BDM-ID of Topic '%s' is too long (%d bytes, max %lu)", 
                    __FUNCTION__, name, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN-1);
            return NULL;
        }

        if(bdm_id) {
            memcpy(bdm_id, p, size);
            bdm_id[size] = '\0';
        }
        p = separator+1;
    }

    return p;
}

int bdm_split_hab_name_r(
        const char * topic,
        char peer_id[BIONET_NAME_COMPONENT_MAX_LEN],
        char bdm_id[BIONET_NAME_COMPONENT_MAX_LEN],
        char hab_type[BIONET_NAME_COMPONENT_MAX_LEN],
        char hab_id[BIONET_NAME_COMPONENT_MAX_LEN])
{
    const char *p;

    p = _remove_bdm_prefix(topic, peer_id, bdm_id);
    if (p == NULL) {
        // Message already logged
        return -1;
    }

    return bionet_split_hab_name_r(p, hab_type, hab_id);
}

int bdm_split_node_name_r(
        const char * topic,
        char peer_id[BIONET_NAME_COMPONENT_MAX_LEN],
        char bdm_id[BIONET_NAME_COMPONENT_MAX_LEN],
        char hab_type[BIONET_NAME_COMPONENT_MAX_LEN],
        char hab_id[BIONET_NAME_COMPONENT_MAX_LEN],
        char node_id[BIONET_NAME_COMPONENT_MAX_LEN])
{
    const char *p;

    p = _remove_bdm_prefix(topic, peer_id, bdm_id);
    if (p == NULL) {
        // Message already logged
        return -1;
    }

    return bionet_split_node_name_r(p, hab_type, hab_id, node_id);
}

int bdm_split_resource_name_r(
        const char * topic,
        char peer_id[BIONET_NAME_COMPONENT_MAX_LEN],
        char bdm_id[BIONET_NAME_COMPONENT_MAX_LEN],
        char hab_type[BIONET_NAME_COMPONENT_MAX_LEN],
        char hab_id[BIONET_NAME_COMPONENT_MAX_LEN],
        char node_id[BIONET_NAME_COMPONENT_MAX_LEN],
        char resource_id[BIONET_NAME_COMPONENT_MAX_LEN])
{
    const char *p;

    p = _remove_bdm_prefix(topic, peer_id, bdm_id);
    if (p == NULL) {
        // Message already logged
        return -1;
    }

    return bionet_split_resource_name_r(p, hab_type, hab_id, node_id, resource_id);
}
