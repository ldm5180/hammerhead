
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

// This library is free software. You can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as 
// published by the Free Software Foundation, version 2.1 of the License.
// This library is distributed in the hope that it will be useful, but 
// WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details. A copy of the GNU 
// Lesser General Public License v 2.1 can be found in the file named 
// "COPYING.LESSER".  You should have received a copy of the GNU Lesser 
// General Public License along with this library; if not, write to the 
// Free Software Foundation, Inc., 
// 51 Franklin Street, Fifth Floor, 
// Boston, MA 02110-1301 USA.
 
// You may contact the Automation Group at:
// bionet@bioserve.colorado.edu
 
// Dr. Kevin Gifford
// University of Colorado
// Engineering Center, ECAE 1B08
// Boulder, CO 80309
 
// Because BioNet was developed at a university, we ask that you provide
// attribution to the BioNet authors in any redistribution, modification, 
// work, or article based on this library.
 
// You may contribute modifications or suggestions to the University of
// Colorado for the purpose of discussing and improving this software.
// Before your modifications are incorporated into the master version 
// distributed by the University of Colorado, we must have a contributor
// license agreement on file from each contributor. If you wish to supply
// the University with your modifications, please join our mailing list.
// Instructions can be found on our website at 
// http://bioserve.colorado.edu/developers-corner.

#ifndef __BDM_CLIENT_H
#define __BDM_CLIENT_H


#include <stdint.h>

#include <glib.h>

#include "bionet-util.h"
#include "bionet-bdm.h"


/**
 * @file bdm-client.h
 * Describes the Bionet Data Manager API
 */


//
// bdm server listens on this TCP port, clients connect
//

#define BDM_PORT      (11002)


/**
 * @brief Connect to a specific BDM server for querying
 *
 * Connect to the specified BDM server, before calling
 * bdm_get_resource_datapoints()
 *
 * @note This is not for subscribing. See bdm_start() that 
 *
 * @param[in] hostname The hostname or ip address string to connect to
 *
 * @param[in] port The port the server is available in (host-byte-ordered), or
 * 0 to use default
 */
int bdm_connect(char *hostname, uint16_t port); 

/**
 * @brief Reads the bdm file descriptor returned from bdm_start()
 *
 * This function should be called whenever the Client application wants to
 * read from Bionet.  The function will read any pending messages from
 * Bionet and if appropriate call the callback functions.
 *
 * @param[in] timeout This is like the timeout argument to select(2).
 * If NULL, the function will block indefinitely until something is read.
 * If not NULL but the value is zero (0 seconds and 0 microseconds), then
 * the function will return immediately, whether or not anything was read.
 * If the timeout is greater than zero, then the function will block until
 * something is read from the fd, but not longer than the timeout.
 *
 * @retval 0 on success.
 * @retval -1 on failure.
 */
int bdm_read_with_timeout(struct timeval *timeout);

/**
 * @brief Reads the bdm file descriptor returned from bdm_start()
 *
 * This function should be called whenever the Client application wants to
 * read from Bionet.  The function will read any pending messages from
 * Bionet and if appropriate call the callback functions.
 *
 * @param[in] timeout This is like the timeout argument to select(2).
 * If NULL, the function will block indefinitely until something is read.
 * If not NULL but the value is zero (0 seconds and 0 microseconds), then
 * the function will return immediately, whether or not anything was read.
 * If the timeout is greater than zero, then the function will block until
 * something is read from the fd, but not longer than the timeout.

 * @param[in] num The maximum number of events to process. Each event usually
 * will invoke a single callback, but not necessarily. The special case value
 * of 0 will process all available events.
 *
 * @retval 0 on success.
 * @retval -1 on failure.
 */
int bdm_read_many(struct timeval *timeout, unsigned int num);


/**
 * @brief Reads the bionet file descriptor returned from bdm_start()
 *
 * This function should be called whenever the Bionet file
 * descriptor returned from bdm_start() is readable, or
 * if the Client application wants to poll the file descriptor.
 * The function will read any pending messages from Bionet and
 * if appropriate call the callback functions.
 *
 * @retval TRUE (non-zero) - success
 * @retval FALSE (zero) - failure.
 */
int bdm_read(void);


/**
 * @brief Checks to see if currently connected to a BDM Server for querying
 *
 * @retval TRUO (non-zero) - the library is connected
 * @retval FALSE (0) - the library is not connected
 */
int bdm_is_connected(void);

/**
 * Connect to Bionet BDM network, and start looking for published BDM servers
 *
 * Calling this function from a BDM Client is optional, it will be called 
 * implicitly when needed by other Bionet BDM library functions.
 *
 * @note If the connection is already opened, this function does nothing and just returns the file descriptor.
 *
 * @retval >=0 success, a non-blocking file descriptor.
 * @retval -1 failure, check errno
 *
 *  @note file descriptor is associated with the Bionet BDM network.  The file 
 * descriptor should not be read or written directly by the Client.
 * If the file descriptor is readable, the Client should call bdm_read()
 * to service it.  Since the fd is non-blocking, the client may also call
 * bdm_read() in a polling way, though this is less efficient than using
 * select() or poll() on the fd.
 */
int bdm_start(void);

/**
 * @brief Disconnect from the BDM server.
 *
 * Disconnect from server after connecting with bdm_connect()
 */
void bdm_disconnect(void);

typedef struct bdm_hab_list_t_opaque bdm_hab_list_t;

/**
 * @brief Query a bionet data manager for a list of datapoints
 *
 * Query the bionet-data-manager connected to with bdm_connect() to 
 * get all datapoints that match the query parameters that are currently
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
bdm_hab_list_t *bdm_get_resource_datapoints(
       const char *resource_name_pattern, 
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
 * The range of indexes is 0 to bdm_get_hab_list_len() - 1
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
 * @return The sequence number of the most recently entered datapoint fetched,
 *         or 0 if no datapoints were fetched in the most recent query.
 */
int bdm_get_hab_list_last_entry_seq(bdm_hab_list_t * hab_list);


/**
 * @brief Free the HAB List
 * 
 * @param[in] hab_list The HAB List to free.
 *
 * @note This causes bionet_hab_free() to be called for each item in the list.
 */
void bdm_hab_list_free(bdm_hab_list_t * hab_list);



/**
 * @brief Register new-bdm callback function with the Bionet library.
 * 
 * The registered function will be called when a new BDM Server is discovered
 *
 * @param[in] cb_new_bdm the "new-bdm" callback function
 *
 * @param[in] usr_data A pointer to user data that will be passed into the function registered
 */
void bdm_register_callback_new_bdm(void (*cb_new_bdm)(bionet_bdm_t *bdm, void * usr_data), void * usr_data);

/**
 * @brief Register lost-bdm callback function with the Bionet library.
 * 
 * The registered function will be called when a BDM Server is no longer
 * available
 *
 * @param[in] cb_lost_bdm The "lost-bdm" callback function
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
void bdm_register_callback_new_hab(void (*cb_new_hab)(bionet_hab_t *hab, bionet_event_t * event, void* usr_data), void*usr_data);


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
void bdm_register_callback_lost_hab(void (*cb_lost_hab)(bionet_hab_t *hab, bionet_event_t * event, void* usr_data), void*usr_data);


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
void bdm_register_callback_new_node(void (*cb_new_node)(bionet_node_t *node, bionet_event_t * event, void* usr_data), void*usr_data);


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
void bdm_register_callback_lost_node(void (*cb_lost_node)(bionet_node_t *node, bionet_event_t * event, void* usr_data), void*usr_data);


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
void bdm_register_callback_datapoint(void (*cb_datapoint)(bionet_datapoint_t *datapoint, bionet_event_t * event, void* usr_data), void*usr_data);


#if 0
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


/**
 * @brief Subscribes the BDM client to the list of BDMs matching the pattern
 *
 * When any BDMs matching the specified pattern join or leave, that fact will
 * be reported to the client via the registered callbacks
 *
 * @param[in] bdm_name A String in the form "[<Peer-ID>,]<BDM-ID>" where any
 * component may be the wildcard "*".
 * Optional components default to "*"
 *
 * @retval 0 Success
 * @retval -1 Failure
 */ 
int bdm_subscribe_bdm_list_by_name(const char * bdm_name);

/**
 * @brief Subscribes the BDM client to the list of HABs matching the pattern
 *
 * When any HABs matching the specified pattern join or leave, that fact will
 * be reported to the client via the registered callbacks
 *
 * @param[in] hab_name A String in the form 
 * "[[<Peer-ID>,]<BDM-ID>/]<HAB-Type>.<HAB-ID>" where any
 * component may be the wildcard "*".
 * Optional components default to "*"
 *
 * @retval 0 Success
 * @retval -1 Failure
 */ 
int bdm_subscribe_hab_list_by_name(const char * hab_name);

/**
 * @brief Subscribes the BDM client to the list of Nodes matching the pattern
 *
 * When any Nodes matching the specified pattern join or leave, that fact will
 * be reported to the client via the registered callbacks
 *
 * @param[in] node_name A String in the form 
 * "[[<Peer-ID>,]<BDM-ID>/]<HAB-Type>.<HAB-ID>.<Node-ID>" where any
 * component may be the wildcard "*".
 * Optional components default to "*"
 *
 *
 * @retval 0 Success
 * @retval -1 Failure
 */ 
int bdm_subscribe_node_list_by_name(const char * node_name);

/**
 * @brief Subscribes the BDM client to the list of datapoints matching the pattern
 *
 * When any datapoints matching the specified pattern are published, that fact will
 * be reported to the client via the registered callbacks
 *
 * @param[in] resource_name A String in the form 
 * "[[<Peer-ID>,]<BDM-ID>/]<HAB-Type>.<HAB-ID>.<Node-ID>:<Resource-ID>" where any
 * component may be the wildcard "*".
 * Optional components default to "*"
 *
 * @param[in] start_time Only report datapoints with a timestamp after this value. May be NULL
 * for no start filter
 *
 * @param[in] stop_time Only report datapoints with a timestamp before this value. May be NULL
 * for no stop filter
 *
 * @retval 0 Success
 * @retval -1 Failure
 *
 * @note Subscribing without specifying a start_time or a time range which is very large is 
 * potentially dangerous. It will cause all matching datapoints from the beginning of time in the 
 * bionet-data-manager database to be published. This could take a very long time, be taxing on the 
 * performance of the bionet-data-manager, and create unnecessary network traffic.
 * Use gettimeofday() to populate start/stop time if you want the current time.
 * @code
    struct timeval tv;
    gettimeofday(&tv, NULL);
    bdm_subscribe_datapoints_by_name(res_name, &tv, NULL) //subscribe all datapoints from now on
 * @endcode
 */ 
int bdm_subscribe_datapoints_by_name(const char * resource_name, 
        struct timeval *start_time, struct timeval *stop_time);


/**
 * @brief Removes a subscription to a list of datapoints.
 *
 * If the specified pattern matches a subscription, the matched subscription
 * is removed (the client will no longer be notified when matching datapoints
 * join or leave the network). Otherwise, this function does nothing and
 * returns failure, check errno:
 *
 * EINVAL  = Invalid hab name
 *
 * ENOENT  = Matching subscription does not exist
 *
 * @param resource_name A string in the form 
 * "<HAB-Type>.<HAB-ID>.<Node-ID>:<Resource-ID>" where any component may be
 * the wildcard "*".
 *
 * @param[in] start_time Only report datapoints with a timestamp after this value. May be NULL
 * for no start filter
 *
 * @param[in] stop_time Only report datapoints with a timestamp before this value. May be NULL
 * for no stop filter
 *
 * @retval 0 Success
 * @retval -1 Error
 */
int bdm_unsubscribe_datapoints_by_name(const char *resource_name,
				       struct timeval *start_time, struct timeval *stop_time);


/**
 * @brief Get the number of BDMs available
 * 
 * @return Number of available BDMs
 */
unsigned int bdm_cache_get_num_bdms(void);


/**
 * @brief Get a BDM by its index
 *
 * Used for iterating over all known BDM
 *
 * @param[in] index Index of the BDM requested
 *
 * @return Pointer to the BDM requested or NULL if no BDM exists at index
 */
bionet_bdm_t *bdm_cache_get_bdm_by_index(unsigned int index);


/**
 * @brief Looks through the locally cached information for a specific BDM.
 *
 * @param[in] bdm_id The BDM-ID to look up
 *
 * @return Pointer to the BDM if found
 * @retval NULL Not found
 */
bionet_bdm_t *bdm_cache_lookup_bdm(const char *bdm_id);


/**
 * @brief Get the number of HABs available
 * 
 * @return Number of available HABs
 */
unsigned int bdm_cache_get_num_habs(void);


/**
 * @brief Get a HAB by its index
 *
 * Used for iterating over all known HABs
 *
 * @param[in] index Index of the HAB requested
 *
 * @return Pointer to the HAB requested or NULL if no HAB exists at index
 */
bionet_hab_t *bdm_cache_get_hab_by_index(unsigned int index);


/**
 * @brief Looks through the locally cached information for a specific HAB.
 *
 * @param[in] hab_type The HAB-Type to look up
 * @param[in] hab_id The HAB-ID to look up
 *
 * @return Pointer to the HAB if found
 * @retval NULL Not found
 */
bionet_hab_t *bdm_cache_lookup_hab(const char *hab_type, const char *hab_id);


/**
 * @brief Looks through the locally cached information for a specific Node.
 *
 * @param[in] hab_type The HAB-Type to look up
 * @param[in] hab_id The HAB-ID to look up
 * @param[in] node_id The Node-ID to look up
 *
 * @return Pointer to the NULL if found
 * @retval NULL Not found
 */
bionet_node_t *bdm_cache_lookup_node(const char *hab_type, const char *hab_id,
				    const char * node_id);


/**
 * @brief Looks through the locally cached information for a specific Resource.
 *
 * @param[in] hab_type The HAB-Type to look up
 * @param[in] hab_id The HAB-ID to look up
 * @param[in] node_id The Node-ID to look up
 * @param[in] resource_id The Resource-ID to look up
 *
 * @return Pointer to the NULL if found
 * @retval NULL Not found
 */
bionet_resource_t *bdm_cache_lookup_resource(const char *hab_type, const char *hab_id,
					     const char * node_id,
					     const char * resource_id);


#endif

