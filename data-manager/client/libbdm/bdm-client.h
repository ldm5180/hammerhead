
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef __BDM_CLIENT_H
#define __BDM_CLIENT_H


#include <stdint.h>

#include <glib.h>

#include "bionet-asn.h"


/**
 * @file bdm-client.h
 * Describes the Bionet Data Manager API
 */

typedef struct bionet_bdm_t_opaque bionet_bdm_t;

/**
 * @brief Connect to a specific BDM server
 *
 * Connect to the specified BDM server, and act as if it has been discovered.
 * Usefull the the BDM server is not on the link-local network
 *
 * @param[in] hostname The hostname or ipaddress string to connect to
 *
 * @param[in] port The port the server is available in (host-byte-ordered), or
 * 0 to use default
 */
int bdm_add_server(char *hostname, uint16_t port); int bdm_is_connected(void);

/**
 * Connect to Bionet BDM network
 *
 * Calling this function from a BDM Client is optional, it will be called implicitly when needed by other Bionet BDM library finctions.
 *
 * @note If the connection is already opened, this function does nothing and just returns the file descriptor.
 *
 * @retval >=0 success, a non-blocking file descriptor.
 * @retval -1 failure, check errno
 *
 *  @note file descriptor is associated with the Bionet network.  The file 
 * descriptor should not be read or written directly by the Client.
 * If the file descriptor is readable, the Client should call bionet_read()
 * to service it.  Since the fd is non-blocking, the client may also call
 * bionet_read() in a polling way, though this is less efficient than using
 * select() or poll() on the fd.
 */
int bdm_connect(void);

/**
 * @brief Checks to see if Bionet BDM library is connected to Bionet BDM network
 *
 * @retval TRUO (non-zero) - the library is connected
 * @retval FALSE (0) - the library is not connected
 */

/**
 * @brief Disconnect from the bionet BDM network and free all resources
 *
 * Dissconect from the Bionet BDM network and free all resources. This will
 * trigger 'lost' messages for any active subscriptions
 */
void bdm_disconnect(void);

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
 *
 * @param[in] datapointStart If not NULL, return only datapoints with a
 *   timestamp after this
 *
 * @param[in] datapointEnd If not NULL, return only datapoints with a timestamp
 *   before this
 *
 * @param[in] entryStart If not NULL, return only datapoints with an entry
 *   sequence number greater or equal to this
 *
 * @param[in] entryEnd If not NULL, return only datapoints with an entry
 *   sequence number less than or equal to this
 */
GPtrArray *bdm_get_resource_datapoints(const char *resource_name_pattern, 
				       struct timeval *datapointStart, 
				       struct timeval *datapointEnd,
				       int entryStart,
				       int entryEnd);


/**
 * @brief Register new-bdm callback function with the Bionet library.
 * 
 * The registered function will be called when a new BDM Server is discovered
 *
 * @param[in] cb_new_bdm the "new-bdm" callback function
 *
 * @param[in] usr_data A pointer to user data that will be passed into the function registered
 */
void bdm_register_callback_new_bdm(void (*cb_new_bdm)(bionet_bdm_t *bdm, void * use_data), void * usr_data);

/**
 * @brief Register lost-bdm callback function with the Bionet library.
 * 
 * The registered function will be called when a BDM Server is no longer
 * available
 *
 * @param[in] cb_new_bdm the "new-bdm" callback function
 *
 * @param[in] usr_data A pointer to user data that will be passed into the function registered
 *
 */
void bdm_register_callback_lost_bdm(void (*cb_lost_bdm)(bionet_bdm_t *bdm, void* usr_data), void*usr_data);

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
 *
 * @param[in] usr_data A pointer to user data that will be passed into the function registered
 */
void bdm_register_callback_new_hab(void (*cb_new_hab)(bionet_hab_t *hab, void* usr_data), void*usr_data);


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
 *
 * @param[in] usr_data A pointer to user data that will be passed into the function registered
 */
void bdm_register_callback_lost_hab(void (*cb_lost_hab)(bionet_hab_t *hab, void* usr_data), void*usr_data);


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
 *
 * @param[in] usr_data A pointer to user data that will be passed into the function registered
 */
void bdm_register_callback_new_node(void (*cb_new_node)(bionet_node_t *node, void* usr_data), void*usr_data);


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
 *
 * @param[in] usr_data A pointer to user data that will be passed into the function registered
 */
void bdm_register_callback_lost_node(void (*cb_lost_node)(bionet_node_t *node, void* usr_data), void*usr_data);


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
 *
 * @param[in] usr_data A pointer to user data that will be passed into the function registered
 */
void bdm_register_callback_datapoint(void (*cb_datapoint)(bionet_datapoint_t *datapoint, void* usr_data), void*usr_data);


/**
 * @brief Registers Stream callback function with the Bionet library.
 *         
 * The 'cb_stream' function gets called when information is published on a
 * Stream matching a Stream subscription.  Stream subscriptions are created
 * with bdm_subscribe_stream().
 *
 * @param[in] cb_stream The new callback function.
 *
 * @param[in] usr_data A pointer to user data that will be passed into the function registered
 */
void bdm_register_callback_stream(void (*cb_stream)(bionet_stream_t *stream, void *buffer, int size, void* usr_data), void*usr_data);


#endif

