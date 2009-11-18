
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <glib.h>

#include "bionet-util.h"
#include "libbdm-internal.h"

/* BDM Cache: Manage memory for the bionet data tree repersenting all the data
 * that has been sent to the user
 *
 * Notify user of lost entities before they are freed, if the user subscribed to them
 */


static int _is_subscribed_to_bdm(bionet_bdm_t * bdm) {
    GSList * li;
    for (li = libbdm_bdm_subscriptions; li != NULL; li = li->next) {
        libbdm_bdm_subscription_t *sub = li->data;

        if (bionet_bdm_matches_id(bdm, sub->bdm_id)) {
            return 1;
        }
    }
    return 0;
}


static int _is_subscribed_to_hab(
        const char * bdm_id,
        const char * hab_type,
        const char * hab_id)
{
    GSList * li;
    for (li = libbdm_hab_subscriptions; li != NULL; li = li->next) {
        libbdm_hab_subscription_t *sub = li->data;

        if ( bionet_name_component_matches(bdm_id, sub->bdm_id)
        &&   bionet_name_component_matches(hab_type, sub->hab_type)
        &&   bionet_name_component_matches(hab_id, sub->hab_id) )
        {
            return 1;
        }
    }
    return 0;
}

static int _is_subscribed_to_node(
        const char * bdm_id,
        const char * hab_type,
        const char * hab_id,
        const char * node_id)
{
    GSList * li;
    for (li = libbdm_node_subscriptions; li != NULL; li = li->next) {
        libbdm_node_subscription_t *sub = li->data;

        if ( bionet_name_component_matches(bdm_id, sub->bdm_id)
        &&   bionet_name_component_matches(hab_type, sub->hab_type)
        &&   bionet_name_component_matches(hab_id, sub->hab_id)
        &&   bionet_name_component_matches(node_id, sub->node_id) )
        {
            return 1;
        }
    }
    return 0;
}


static void _bdmcache_delete_node(
        bionet_node_t * node,
        const char * bdm_id,
        const char * hab_type,
        const char * hab_id)
{
    if(libbdm_callback_lost_node && _is_subscribed_to_node(bdm_id, hab_type, hab_id, node->id )) {
        libbdm_callback_lost_node(node, libbdm_callback_lost_node_usr_data);
    }

    bionet_node_free(node);
}

// bdm_id is the parent that is leaving (if any) used to determin if this 
// hab was subscribed to...
static void _bdmcache_delete_hab(bionet_hab_t * hab, const char * bdm_id) {
    GSList *li;

    libbdm_habs = g_slist_remove(libbdm_habs, hab);

    if ( bdm_id) {
        for (li = hab->nodes; li != NULL; li = li->next) {
            bionet_node_t *node = li->data;

            _bdmcache_delete_node(node, bdm_id, hab->type, hab->id);
        }
    }
    g_slist_free(hab->nodes);
    hab->nodes = NULL;

    if(bdm_id && libbdm_callback_lost_hab && _is_subscribed_to_hab(bdm_id, hab->type, hab->id)) {
        libbdm_callback_lost_hab(hab, libbdm_callback_lost_hab_usr_data);
    }

    for (li = hab->bdms; li != NULL; li = li->next) {
        bionet_bdm_t *bdm = li->data;

        bdm->habs = g_slist_remove(bdm->habs, hab);
    }

    bionet_hab_free(hab);
}

static void _bdmcache_delete_bdm(bionet_bdm_t * bdm) {
    GSList *li;

    for (li = bdm->habs; li != NULL; li = li->next) {
        bionet_hab_t *hab = li->data;

        hab->bdms = g_slist_remove(hab->bdms, bdm);

        if(bionet_hab_get_num_bdms(hab) == 0) {
            _bdmcache_delete_hab(hab, bdm->id);
        }
    }
    g_slist_free(bdm->habs);
    bdm->habs = NULL;

    if(libbdm_callback_lost_bdm && _is_subscribed_to_bdm(bdm)) {
        libbdm_callback_lost_bdm(bdm, libbdm_callback_lost_bdm_usr_data);
    }

    // Don't free this, because it is still in libbdm_all_peers
    //bionet_bdm_free(bdm);
}


void libbdm_cache_add_bdm(bionet_bdm_t *bdm) {
    libbdm_bdms = g_slist_prepend(libbdm_bdms, bdm);
}


void libbdm_cache_remove_bdm(bionet_bdm_t *bdm) {
    libbdm_bdms = g_slist_remove(libbdm_bdms, bdm);
    _bdmcache_delete_bdm(bdm);
}


void libbdm_cache_add_hab(bionet_hab_t *hab) {
    libbdm_habs = g_slist_prepend(libbdm_habs, hab);
}


void libbdm_cache_remove_hab(bionet_hab_t *hab) {
    _bdmcache_delete_hab(hab, NULL);
}


void libbdm_cache_add_node(bionet_node_t *node) {
    if (NULL == node) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libbdm_cache_add_node(): NULL node passed in");
	errno = EINVAL;
	return;
    }
    bionet_hab_t *hab;
    bionet_hab_t *nhab = bionet_node_get_hab(node);
    if (NULL == nhab)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libbdm_cache_add_node(): Failed to get hab from node.");
	return;
    }

    hab = bdm_cache_lookup_hab(bionet_hab_get_type(nhab), bionet_hab_get_id(nhab));
    if (hab == NULL) {
        hab = bionet_hab_new(bionet_hab_get_type(nhab), bionet_hab_get_id(nhab));
        libbdm_cache_add_hab(hab);
    }

    if (bionet_hab_add_node(hab, node)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libbdm_cache_add_node(): Failed to add node to hab.");
    }
}


void libbdm_cache_remove_node(bionet_node_t *node) {
    if (NULL == node) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libbdm_cache_remove_node(): NULL node passed in");
	errno = EINVAL;
	return;
    }
    bionet_hab_t *hab;
    bionet_hab_t *nhab = bionet_node_get_hab(node);
    if (NULL == nhab)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libbdm_cache_add_node(): Failed to get hab from node.");
	return;
    }

    hab = bdm_cache_lookup_hab(bionet_hab_get_type(nhab), bionet_hab_get_id(nhab));
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "cache tried to remove a node for a non-existent HAB (%s.%s.%s)", 
	      bionet_hab_get_type(hab), bionet_hab_get_id(hab), bionet_node_get_id(node));
        return;
    }

    bionet_hab_remove_node_by_id(hab, bionet_node_get_id(node));
}


void libbdm_cache_add_resource(bionet_resource_t *resource) {
    bionet_node_t *node;

    if (NULL == resource) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "libbdm_cache_add_resource(): NULL resource passed in");
	errno = EINVAL;
	return;
    }
    bionet_node_t * rnode = bionet_resource_get_node(resource);
    bionet_hab_t * rhab = bionet_node_get_hab(rnode);
    node = bdm_cache_lookup_node(bionet_hab_get_type(rhab),
				    bionet_hab_get_id(rhab),
				    bionet_node_get_id(rnode));
    if (NULL == node) {
	node = bionet_node_new(rhab, bionet_node_get_id(rnode));
	libbdm_cache_add_node(node);
    }

    if (bionet_node_add_resource(node, resource)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "Failed to add resource %s to node %s",
	      bionet_resource_get_id(resource),
	      bionet_node_get_name(node));
    }
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
