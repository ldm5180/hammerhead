// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef __BDM_CLIENT_INTERNAL_H
#define __BDM_CLIENT_INTERNAL_H

#include "bdm-client.h"
#include "cal-client.h"
#include "internal.h"
#include "bionet-bdm.h"
#include "bionet-asn.h"

#define BDM_DEFAULT_HOST "localhost"

#define UUID_FMTSTR "%02x%02x%02x%02x%02x%02x%02x%02x"
#define UUID_ARGS(x) x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7] 

// If using gcc, use attributes
#ifdef __GNUC__
#define WARN_UNUSED_RESULT __attribute__ ((warn_unused_result))
#else
#define WARN_UNUSED_RESULT
#endif

extern bdm_hab_list_t * _libbdm_query_response;

int bdm_send_asn(const void *buffer, size_t size, void *unused);

void libbdm_cal_callback(void * cal_handle, const cal_event_t *event);
void bdm_handle_query_response(const cal_event_t *event,
        ResourceDatapointsReply_t *rdr);

extern void (*libbdm_callback_new_bdm)(bionet_bdm_t *bdm, void* usr_data);
extern void * libbdm_callback_new_bdm_usr_data;
extern void (*libbdm_callback_lost_bdm)(bionet_bdm_t *bdm, void* usr_data);
extern void * libbdm_callback_lost_bdm_usr_data;

extern void (*libbdm_callback_new_hab)(bionet_hab_t *hab, bionet_event_t * event, void* usr_data);
extern void * libbdm_callback_new_hab_usr_data;
extern void (*libbdm_callback_lost_hab)(bionet_hab_t *hab, bionet_event_t * event, void* usr_data);
extern void * libbdm_callback_lost_hab_usr_data;

extern void (*libbdm_callback_new_node)(bionet_node_t *node, bionet_event_t * event, void* usr_data);
extern void * libbdm_callback_new_node_usr_data;
extern void (*libbdm_callback_lost_node)(bionet_node_t *node, bionet_event_t * event, void* usr_data);
extern void * libbdm_callback_lost_node_usr_data;

extern void (*libbdm_callback_datapoint)(bionet_datapoint_t *datapoint, bionet_event_t * event, void* usr_data);
extern void * libbdm_callback_datapoint_usr_data;

extern void (*libbdm_callback_stream)(bionet_stream_t *stream, void *buffer, int size, void* usr_data);
extern void * libbdm_callback_stream_usr_data;


extern void * libbdm_cal_handle;

int bdm_subscribe_datapoints_by_bdmid_habtype_habid_nodeid_resourceid(
        const char *peer_id,
        const char *bdm_id,
        const char *hab_type,
        const char *hab_id,
        const char *node_id,
        const char *resource_id,
        struct timeval *datapoint_start,
        struct timeval *datapoint_end) ;

// Split the bdm pattern in the form [peer-id,]bdm-id
// peer-id is '*' if not specified
extern int bdm_split_bdm_peer_id(
        const char * name,
        char peer_id[BIONET_NAME_COMPONENT_MAX_LEN],
        char bdm_id[BIONET_NAME_COMPONENT_MAX_LEN] );

// Backword compatibility for old client/server interface
extern int bdm_fd;

typedef struct {
    char * bdm_id;
    long curr_seq; // The sequence for all completed subscriptions
    GHashTable * new_seq_by_topic; // The starting secuence for new subscriptions
} libbdm_peer_t;

extern GHashTable *libbdm_all_peers; // All peers reported by CAL. Table of libbdm_peer_t pointers

extern GSList *libbdm_bdms; // List of all bdms. This is the manager of BDM memory.
extern GSList *libbdm_habs; // List of all habs. This is the manager of Hab memory

extern GSList *libbdm_nodes; // List of all nodes. This is NOT the manager, just a list to lookup into

typedef struct {
    char *peer_id;
} libbdm_bdm_subscription_t;

// 
// these data structures track the client's registered subscriptions
//

typedef struct {
    char *peer_id;
    char *bdm_id;
    char *hab_type;
    char *hab_id;
    struct timeval *timestamp_start;
    struct timeval *timeval_end;
} libbdm_hab_subscription_t;


typedef struct {
    char *peer_id;
    char *bdm_id;
    char *hab_type;
    char *hab_id;
    char *node_id;
    struct timeval *timestamp_start;
    struct timeval *timeval_end;
} libbdm_node_subscription_t;


typedef struct {
    char *peer_id;
    char *bdm_id;
    char *hab_type;
    char *hab_id;
    char *node_id;
    char *resource_id;
    struct timeval *timestamp_start;
    struct timeval *timeval_end;
} libbdm_datapoint_subscription_t;

extern GSList *libbdm_bdm_subscriptions;
extern GSList *libbdm_hab_subscriptions;
extern GSList *libbdm_node_subscriptions;
extern GSList *libbdm_datapoint_subscriptions;
extern GSList *libbdm_stream_subscriptions;

//
// functions for dealing with the cache
//

void libbdm_cache_add_bdm(bionet_bdm_t *bdm);
void libbdm_cache_remove_bdm(bionet_bdm_t *bdm);

// These may free the pointer passed in, and return a different one
void libbdm_cache_add_hab(bionet_hab_t *hab);
void libbdm_cache_add_node(bionet_node_t *node);

void libbdm_cache_remove_hab(bionet_hab_t *hab);
void libbdm_cache_remove_node(bionet_node_t *node);

void libbdm_cache_add_resource(bionet_resource_t *resource);
void libbdm_cache_replace_resource(bionet_resource_t *resource);

void libbdm_cache_cleanup_habs();
void libbdm_cache_cleanup_nodes();

/**
 * @brief Looks through the locally cached information for a specific Node.
 *
 * @param[in] hab_type The HAB-Type to look up
 * @param[in] hab_id The HAB-ID to look up
 * @param[in] node_id The Node-ID to look up
 *
 * @return Pointer to the Node if found
 * @retval NULL Not found
 */
bionet_node_t *bdm_cache_lookup_node(const char *hab_type, const char *hab_id, const char *node_id);

/**
 * @brief Looks through the locally cached information for a specific Node.
 *
 * @param[in] node_uid The UUID of the node to return
 *
 * @return Pointer to the Node if found
 * @retval NULL Not found
 */
bionet_node_t *bdm_cache_lookup_node_uid(const uint8_t node_uid[BDM_UUID_LEN]);

/**
 * @brief Looks through the locally cached information for a specific Resource
 *
 * @param[in] node_uid The UUID of the node to return
 * @param[in] resource_id The Resource-ID to look up
 *
 * @return Pointer to the Resource if found
 * @retval NULL Not found
 */
bionet_resource_t *bdm_cache_lookup_resource_uid(const uint8_t node_uid[BDM_UUID_LEN], const char *resource_id);



#if 0
/**
 * @brief Looks through the locally cached information for a specific Stream.
 *
 * @param[in] hab_type The HAB-Type to look up
 * @param[in] hab_id The HAB-ID to look up
 * @param[in] node_id The Node-ID to look up
 * @param[in] resource_id The Resource-ID to look up
 *
 * @return Pointer to the stream if found
 * @retval NULL Not found
 */
bionet_stream_t *bdm_cache_lookup_stream(const char *hab_type, const char *hab_id, const char *node_id, const char *resource_id);
#endif


#endif
