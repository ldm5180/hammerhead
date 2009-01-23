
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#define _GNU_SOURCE // for strndup(3)


#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "libbionet-internal.h"
#include "bionet.h"

#include "bionet-asn.h"
#include "bionet-util.h"



int bionet_set_resource(bionet_resource_t *resource, const char *value) {
    bionet_node_t *node;
    bionet_hab_t *hab;

    /* sanity */
    if (NULL == resource) {
	errno = EINVAL;
	return -1;
    }

    node = bionet_resource_get_node(resource);
    hab = bionet_node_get_hab(node);

    return bionet_set_resource_by_habtype_habid_nodeid_resourceid(bionet_hab_get_type(hab), 
								  bionet_hab_get_id(hab), 
								  bionet_node_get_id(node), 
								  bionet_resource_get_id(resource), 
								  value);
}


int bionet_set_resource_by_name_pattern(const char *resource_name_pattern, const char *value) {
    char *hab_type;
    char *hab_id;
    char *node_id;
    char *resource_id;
    int r;

    r = bionet_split_resource_name(resource_name_pattern, &hab_type, &hab_id, &node_id, &resource_id);
    if (r != 0) {
        // a helpful log message has already been logged
        return -1;
    }

    return bionet_set_resource_by_habtype_habid_nodeid_resourceid(hab_type, hab_id, node_id, resource_id, value);
}


int bionet_set_resource_by_habtype_habid_nodeid_resourceid(
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    const char *value
) {
    bionet_asn_buffer_t buf;
    char peer_name[(BIONET_NAME_COMPONENT_MAX_LEN * 2) + 2];
    C2H_Message_t m;
    SetResourceValue_t *s;
    int r;
    asn_enc_rval_t asn_r;

    memset(&m, 0, sizeof(C2H_Message_t));
    memset(&buf, 0, sizeof(bionet_asn_buffer_t));

    m.present = C2H_Message_PR_setResourceValue;
    s = &m.choice.setResourceValue;

    r = OCTET_STRING_fromString(&s->nodeId, node_id);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_set_resource_by_habtype_habid_nodeid_resourceid(): error making OCTET_STRING for Node-ID %s", node_id);
        goto cleanup;;
    }

    r = OCTET_STRING_fromString(&s->resourceId, resource_id);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_set_resource_by_habtype_habid_nodeid_resourceid(): error making OCTET_STRING for Resource-ID %s", resource_id);
        goto cleanup;
    }

    r = OCTET_STRING_fromString(&s->value, value);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_set_resource_by_habtype_habid_nodeid_resourceid(): error making OCTET_STRING for Value %s", value);
        goto cleanup;
    }


    //
    // serialize the ASN.1 message
    //

    asn_r = der_encode(&asn_DEF_C2H_Message, &m, bionet_accumulate_asn_buffer, &buf);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_C2H_Message, &m);
    if (asn_r.encoded == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_set_resource_by_habtype_habid_nodeid_resourceid(): error with der_encode(): %s", strerror(errno));
        goto cleanup;
    }


    sprintf(peer_name, "%s.%s", hab_type, hab_id);

    // send the command to the HAB
    // Note: cal_client.sendto steals the dynamically-allocated buffer
    cal_client.sendto(peer_name, buf.buf, buf.size);

    return 0;


cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_C2H_Message, &m);
    if (buf.buf != NULL) free(buf.buf);
    return -1;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
