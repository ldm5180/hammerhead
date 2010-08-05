
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include "protected.h"

#include <string.h>
#include <openssl/sha.h>

#define UUID_FMTSTR "%02x%02x%02x%02x%02x%02x%02x%02x"
#define UUID_ARGS(x) x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7] 

int db_make_node_guid(
    bionet_node_t * node,
    uint8_t guid[BDM_UUID_LEN])
{
    SHA_CTX sha_ctx;
    unsigned char sha_digest[SHA_DIGEST_LENGTH];
    int r;
    uint8_t byte;

    bionet_hab_t * hab = bionet_node_get_hab(node);
    const char * hab_type = bionet_hab_get_type(hab);
    const char * hab_id = bionet_hab_get_id(hab);
    const char * node_id = bionet_node_get_id(node);


    bionet_node_sort_resources(node, NULL);

    r = SHA1_Init(&sha_ctx);
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error initializing SHA1 context");
        return -1;
    }

    r = SHA1_Update(&sha_ctx, hab_type, strlen(hab_type));
    if (r != 1) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource HAB-Type");
	return -1;
    }
    
    r = SHA1_Update(&sha_ctx, hab_id, strlen(hab_id));
    if (r != 1) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource HAB-ID");
	return -1;
    }

    r = SHA1_Update(&sha_ctx, node_id, strlen(node_id));
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource Node-ID");
        return -1;
    }

    int i;
    for(i=0; i<bionet_node_get_num_resources(node); i++) {
        bionet_resource_t * resource = bionet_node_get_resource_by_index(node, i);


        const char * resource_id = bionet_resource_get_id(resource);
        bionet_resource_data_type_t data_type = bionet_resource_get_data_type(resource);
        bionet_resource_flavor_t flavor = bionet_resource_get_flavor(resource);

        r = SHA1_Update(&sha_ctx, resource_id, strlen(resource_id));
        if (r != 1) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource ID");
            return -1;
        }

        byte = data_type;
        r = SHA1_Update(&sha_ctx, &byte, 1);
        if (r != 1) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource Data Type");
            return -1;
        }

        byte = flavor;
        r = SHA1_Update(&sha_ctx, &byte, 1);
        if (r != 1) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource Flavor");
            return -1;
        }
    }

    r = SHA1_Final(sha_digest, &sha_ctx);
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error finalizing SHA1 digest");
        return -1;
    }

    memcpy(guid, sha_digest,BDM_UUID_LEN);

    return 0;

}
