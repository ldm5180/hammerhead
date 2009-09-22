
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef __BDM_H
#define __BDM_H


#include <stdint.h>

#include <glib.h>

#include "bionet-asn.h"


/**
 * @file bdm-client.h
 * Describes the Bionet Data Manager API
 */


// FIXME: switch to CAL and this'll use mDNS-SD
int bdm_connect(char *hostname, uint16_t port);
int bdm_is_connected(void);
void bdm_disconnect(void);


typedef struct bionet_bdm_t_opaque bionet_bdm_t;

typedef struct bdm_hab_list_t_opaque bdm_hab_list_t;

/**
 * @brief Query a bionet data manager for a list of datapoints
 *
 * Get all datapoints that match the query parameters that are currently
 * available in the bionet data manager.
 *
 * @note There may be additional datapoints that match the query parameters
 *   that show up at a later time. To return only the new points, use an 
 *   entry number range greater than the largest number returned in the past
 * 
 * @param[in] resource_name_pattern The resource name pattern to query for
 * @param[in] datapointStart If not NULL, return only datapoints with a
 *            timestamp after this
 * @param[in] datapointEnd If not NULL, return only datapoints with a timestamp
 *            before this
 * @param[in] entryStart If not NULL, return only datapoints with an entry
 *            sequence number greater or equal to this
 * @param[in] entryEnd If not NULL, return only datapoints with an entry
 *            sequence number less than or equal to this
 *
 * @return A HAB List which can be walked to find all the datapoints satisfying 
 *         the query.
 */
bdm_hab_list_t *bdm_get_resource_datapoints(const char *resource_name_pattern, 
				       struct timeval *datapointStart, 
				       struct timeval *datapointEnd,
				       int entryStart,
				       int entryEnd);


/**
 * @brief Get the length of a HAB List
 *
 * @param[in] hab_list The HAB List being queried.
 *
 * @return Number of HABs in the HAB List
 */
int bdm_get_hab_list_len(bdm_hab_list_t * hab_list);


/**
 * @brief Get a HAB from a HAB List
 *
 * The range of indicies is 0 to bdm_get_hab_list_len()
 *
 * @param[in] hab_list The HAB List being queried.
 * @param[in] index Index of the HAB to fetch.
 *
 * @return Ptr to the HAB in requested, or NULL of the index is out of range
 */
bionet_hab_t * bdm_get_hab_by_index(bdm_hab_list_t * hab_list, int index);


/**
 * @brief Get the last entry sequence number from a HAB List
 * 
 * Each time datapoints are fetched from the BDM, this value is updated
 * to reflect the sequence number of the most recent datapoint entered
 * into the database. This can be used when polling the BDM so as to
 * not receive duplicates.
 *
 * @param[in] hab_list The HAB List being queried.
 *
 * @return The sequence number of the most recently entered datapoint fetched.
 */
int bdm_get_hab_list_last_entry_seq(bdm_hab_list_t * hab_list);


/**
 * @brief Free the HAB List
 * 
 * @param[in] hab_list The HAB List to free.
 */
void bdm_hab_list_free(bdm_hab_list_t * hab_list);


int bdm_send_asn(const void *buffer, size_t size, void *unused);


#if 0
/**
 * @brief Register new-bdm callback function with the Bionet library.
 * 
 * The registered function will be called when a new BDM Server is discovered
 *
 * @param[in] cb_new_bdm the "new-bdm" callback function
 */
void bdm_register_callback_new_bdm(void (*cb_new_bdm)(bionet_bdm_t *bdm));

/**
 * @brief Register lost-bdm callback function with the Bionet library.
 * 
 * The registered function will be called when a BDM Server is no longer
 * available
 *
 * @param[in] cb_new_bdm the "new-bdm" callback function
 *
 */
void bdm_register_callback_lost_bdm(void (*cb_lost_bdm)(bionet_bdm_t *bdm));

/**
 * @brief Registers new hab callback function with the Bionet library.
 *         
 * This will be called when information is published that the Client has 
 * subscribed to.
 *
 * @note The "lost hab callback" MUST properly unset and deallocate the 
 * 'user_data' member or the application will leak memory.
 * 
 * @note The "lost hab callback" is called when a HAB that matches a HAB-list
 * subscription leaves the network. 
 *
 * @param[in] cb_new_hab The "new hab" callback function.
 */
void bdm_register_callback_new_hab(void (*cb_new_hab)(bionet_hab_t *hab));


/**
 * @brief Registers lost hab callback function with the Bionet library.
 *         
 * This will be called when information is published that the Client has 
 * subscribed to.
 *
 * The "new hab callback" is called when a HAB that matches a HAB-list
 * subscription joins the network. HAB-list subscriptions are created with
 * the bdm_subscribe_hab_list() function. 
 *
 * @note The bionet_hab_t argument is the property of the Bionet Client 
 * library, and must not be deallocated by the callback functions.  
 *
 * @note The new-hab callback function may set the bionet_hab_t's 'user_data'
 * member.   
 *
 * @param[in] cb_lost_hab The "lost hab" callback function.
 */
void bdm_register_callback_lost_hab(void (*cb_lost_hab)(bionet_hab_t *hab));


/**
 * @brief Registers new node callback function with the Bionet library.
 *         
 * This will be called when information is published that the Client has 
 * subscribed to.
 *
 * The 'cb_new_node' function is called when a Node that matches a 
 * Node-list subscription joins a HAB.  Node-list subscriptions are
 * created with bdm_subscribe_node_list().  
 *
 * @note The bionet_node_t argument is the property
 * of the Bionet Client library, and must not be deallocated
 * by the callback functions.  
 *
 * @note The new-node callback may set
 * the bionet_node_t's 'user_data' member.
 *
 * @param[in] cb_new_node The "new node" callback function.
 */
void bdm_register_callback_new_node(void (*cb_new_node)(bionet_node_t *node));


/**
 * @brief Registers lost node callback function with the Bionet library.
 *         
 * This will be called when information is published that the Client has 
 * subscribed to.
 *
 * The 'cb_lost_node' function is called when a Node that matches a 
 * Node-list subscription leaves a HAB.  Node-list subscriptions are
 * created with bdm_subscribe_node_list().  
 *
 * @note The bionet_node_t argument is the property
 * of the Bionet Client library, and must not be deallocated
 * by the callback functions.  
 *
 * @note The lost-node callback MUST properly unset and deallocate the
 * 'user_data'  member or the application will leak memory.
 *
 * @param[in] cb_lost_node The "lost node" callback function.
 */
void bdm_register_callback_lost_node(void (*cb_lost_node)(bionet_node_t *node));


/**
 * @brief Registers datapoint callback function with the Bionet library.
 *         
 * This will be called when information is published that the Client has 
 * subscribed to.
 *
 * The 'cb_datapoint' function gets called when a Resource
 * matching a Datapoint subscription gets a new value.
 * Datapoint subscriptions are created with bdm_subscribe_datapoint().
 *
 * @note The bionet_datapoint_t argument to the callback function is the
 * property of the Bionet Client library, and MUST not be deallocated by the
 * callback function.
 *
 * @param[in] cb_datapoint The "datapoint" callback function.
 */
void bdm_register_callback_datapoint(void (*cb_datapoint)(bionet_datapoint_t *datapoint));


/**
 * @brief Registers Stream callback function with the Bionet library.
 *         
 * The 'cb_stream' function gets called when information is published on a
 * Stream matching a Stream subscription.  Stream subscriptions are created
 * with bdm_subscribe_stream().
 *
 * @param[in] cb_stream The new callback function.
 */
void bdm_register_callback_stream(void (*cb_stream)(bionet_stream_t *stream, void *buffer, int size));

#endif



#endif

