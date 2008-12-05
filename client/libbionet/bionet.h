
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


// 
// This header file describes the Bionet Client API.
//


#ifndef __BIONET_H
#define __BIONET_H




#include <glib.h>

#include "bionet-util.h"




/**
 * @brief The Client ID is used by Bionet.
 *        It exists only for convenience when logging information
 *
 * @note If this function is not called before connecting to bionet
 *       the default value of 'user@host:port (program-name[pid])' will be used.
 *
 * @note The string passed in is copied into private memory within
 *       the Bionet library and may be freed or overwritten after
 *       the function returns.
 *
 * @param[in] new_id The new Client ID string.
 *
 * @return 0 success
 * @return -1 failure.
 */
int bionet_set_id(const char *new_id);


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
void bionet_register_callback_new_hab(void (*cb_new_hab)(bionet_hab_t *hab));


/**
 * @brief Registers lost hab callback function with the Bionet library.
 *         
 * This will be called when information is published that the Client has 
 * subscribed to.
 *
 * The "new hab callback" is called when a HAB that matches a HAB-list
 * subscription joins the network. HAB-list subscriptions are created with
 * the bionet_subscribe_hab_list() function. 

 * @note The bionet_hab_t argument is the property of the Bionet Client 
 * library, and must not be deallocated by the callback functions.  
 *
 * @note The new-hab callback function may set the bionet_hab_t's 'user_data'
 * member.   
 *
 * @param[in] cb_lost_hab The "lost hab" callback function.
 */
void bionet_register_callback_lost_hab(void (*cb_lost_hab)(bionet_hab_t *hab));


/**
 * @brief Registers new node callback function with the Bionet library.
 *         
 * This will be called when information is published that the Client has 
 * subscribed to.
 *
 * The 'cb_new_node' function is called when a Node that matches a 
 * Node-list subscription joins a HAB.  Node-list subscriptions are
 * created with bionet_subscribe_node_list().  
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
void bionet_register_callback_new_node(void (*cb_new_node)(bionet_node_t *node));


/**
 * @brief Registers new node callback function with the Bionet library.
 *         
 * This will be called when information is published that the Client has 
 * subscribed to.
 *
 * The 'cb_lost_node' function is called when a Node that matches a 
 * Node-list subscription leaves a HAB.  Node-list subscriptions are
 * created with bionet_subscribe_node_list().  
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
void bionet_register_callback_lost_node(void (*cb_lost_node)(bionet_node_t *node));


/**
 * @brief Registers datapoint callback function with the Bionet library.
 *         
 * This will be called when information is published that the Client has 
 * subscribed to.
 *
 * The 'cb_datapoint' function gets called when a Resource
 * matching a Datapoint subscription gets a new value.
 * Datapoint subscriptions are created with bionet_subscribe_datapoint().
 *
 * @note The bionet_datapoint_t argument to the callback function is the
 * property of the Bionet Client library, and MUST not be deallocated by the
 * callback function.
 *
 * @param[in] cb_datapoint The "datapoint" callback function.
 */
void bionet_register_callback_datapoint(void (*cb_datapoint)(bionet_datapoint_t *datapoint));


/**
 * @brief Connects to the Bionet network.  
 *
 * Calling this function from the Client is optional, it will be called
 * implicitly when needed by other Bionet library functions.
 *
 * @note If the connection is already opened, the function does
 * nothing and just returns the file descriptor.
 *
 * @return >0 success, a non-blocking file descriptor.
 * @return -1 failure, check errno.
 *
 * @note file descriptor is associated with the Bionet network.  The file 
 * descriptor should not be read or written directly by the Client.
 * If the file descriptor is readable, the Client should call bionet_read()
 * to service it.  Since the fd is non-blocking, the client may also call
 * bionet_read() in a polling way, though this is less efficient than using
 * select() or poll() on the fd.
 */
int bionet_connect(void);


/**
 * @brief Checks to see if the Bionet library is connected to Bionet.
 *
 * @return TRUE (non-zero) the library IS connected to Bionet 
 * @return FALSE (0) if the library is NOT connected to Bionet.
 */
int bionet_is_connected(void);


/**
 * @brief Reads the bionet file descriptor returned from bionet_read()
 *
 * This function should be called whenever the Bionet file
 * descriptor returned from bionet_connect() is readable, or
 * if the Client application wants to poll the file descriptor.
 * The function will read any pending messages from Bionet and
 * if appropriate call the callback functions.
 *
 * @return TRUE (non-zero) on success
 * @return FALSE (zero) on failure.
 */
int bionet_read(void);


/**
 * @brief Get the number of HABs available
 * 
 * @return Number of available HABs
 */
unsigned int bionet_cache_get_num_habs(void);


/**
 * @brief Get a HAB by its index
 *
 * Used for iterating over all known HABs
 *
 * @param[in] index Index of the HAB requested
 *
 * @return Pointer to the HAB requested or NULL if no HAB exists at index
 */
bionet_hab_t *bionet_cache_get_hab_by_index(unsigned int index);


/**
 * @brief Looks through the locally cached information for a specific HAB.
 *
 * @param[in] hab_type The HAB-Type to look up
 * @param[in] hab_id The HAB-ID to look up
 *
 * @return Pointer to the HAB if found
 * @return NULL if it was not found
 */
bionet_hab_t *bionet_cache_lookup_hab(const char *hab_type, const char *hab_id);


/**
 * @brief Looks through the locally cached information for a specific Node.
 *
 * @param[in] hab_type The HAB-Type to look up
 * @param[in] hab_id The HAB-ID to look up
 * @param[in] node_id The Node-ID to look up
 *
 * @return Pointer to the Node if found
 * @return NULL if it was not found
 */
bionet_node_t *bionet_cache_lookup_node(const char *hab_type, const char *hab_id, const char *node_id);


/**
 * @brief Looks through the locally cached information for a specific Resource
 *
 * @param[in] hab_type The HAB-Type to look up
 * @param[in] hab_id The HAB-ID to look up
 * @param[in] node_id The Node-ID to look up
 * @param[in] resource_id The Resource-ID to look up
 *
 * @return Pointer to the Resource if found
 * @return NULL if it was not found
 */
bionet_resource_t *bionet_cache_lookup_resource(const char *hab_type, const char *hab_id, const char *node_id, const char *resource_id);


/**
 * @brief Looks through the locally cached information for a specific Stream.
 *
 * @param[in] hab_type The HAB-Type to look up
 * @param[in] hab_id The HAB-ID to look up
 * @param[in] node_id The Node-ID to look up
 * @param[in] resource_id The Resource-ID to look up
 *
 * @return Pointer to the stream if found
 * @return NULL if it was not found
 */
bionet_stream_t *bionet_cache_lookup_stream(const char *hab_type, const char *hab_id, const char *node_id, const char *resource_id);


/**
 * @brief Subscribes the client to Bionet's list of HABs matching the
 * specified pattern.  
 *
 * When HABs matching the specified pattern join or leave Bionet, that fact 
 * will be reported to the Client.
 *
 * @param[in] hab_name A string in the form "<HAB-Type>.<HAB-ID>" where any 
 * component may be the wildcard "*"
 *
 * @return 0 Success
 * @return 1 Error
 */

int bionet_subscribe_hab_list_by_name(const char *hab_name);


/**
 * @brief Subscribes the client to Bionet's list of HABs matching the
 * specified pattern.  
 *
 * When HABs matching the specified pattern join or leave Bionet, that fact 
 * will be reported to the Client.
 *
 * @param[in] hab_type A string in the form "<HAB-Type>" or the wildcard "*"
 * @param[in] hab_id A string in the form "<HAB-ID>" or the wildcard "*"
 *
 * @note hab_type and hab_id are joined using the "." character to get hab_name
 *
 * @return 0 Success
 * @return 1 Error
 */
int bionet_subscribe_hab_list_by_habtype_habid(const char *hab_type,  const char *hab_id);


/**
 * @brief Subscribes the client to Bionet's list of Nodes matching
 * the specified pattern.  
 *
 * When Nodes matching the specified pattern join or leave a network, Bionet
 * will report the fact to the Client.
 *
 * @param 'node_name' A string in the form "<HAB-Type>.<HAB-ID>.<Node-ID>"
 * where any component may be the wildcard "*".
 *
 * @return 0 Success
 * @return -1 Error
 */
int bionet_subscribe_node_list_by_name(const char *node_name);

/**
 * @brief Subscribes the client to Bionet's list of Nodes matching
 * the specified pattern.  
 *
 * When Nodes matching the specified pattern join or leave a network, Bionet
 * will report the fact to the Client.
 *
 * @param[in] hab_type string of the form "<HAB-Type>" or the wildcard "*"
 * @param[in] hab_id string of the form "<HAB-ID>" or the wildcard "*"
 * @param[in] node_id string of the form "<Node-ID>" or the wildcard "*"
 *
 * @return 0 Success
 * @return -1 Error
 */
int bionet_subscribe_node_list_by_habtype_habid_nodeid(const char *hab_type,  const char *hab_id, const char *node_id);


/**
 * @brief  Subscribes the client to the values of Resources matching
 * the specified pattern.  
 *
 * When Resource matching the specified pattern changes Bionet will report
 * the fact to the Client.
 *
 * @param resource_name A string in the form 
 *"<HAB-Type>.<HAB-ID>.<Node-ID>:<Resource-ID>" where any component may be
 * the wildcard "*".
 *
 * @return 0 Success
 * @return -1 Error
 */
int bionet_subscribe_datapoints_by_name(const char *resource_name);


/**
 * @brief  Subscribes the client to the values of Resources matching
 * the specified pattern.  
 *
 * When Resource matching the specified pattern changes Bionet will report
 * the fact to the Client.
 *
 * @param[in] hab_type string of the form "<HAB-Type>" or the wildcard "*"
 * @param[in] hab_id string of the form "<HAB-ID>" or the wildcard "*"
 * @param[in] node_id string of the form "<Node-ID>" or the wildcard "*"
 * @param[in] resource_id string of the form "<Resource-ID>" or the wildcard "*"
 *
 * @return 0 Success
 * @return -1 Error
 */
int bionet_subscribe_datapoints_by_habtype_habid_nodeid_resourceid(const char *hab_type,  const char *hab_id, const char *node_id, const char *resource_id);


/**
 * @brief  Sends a message to Bionet requesting that the specified
 * Resource be set to the specified value.
 *
 * @param[in] resource Resource to set
 * @param[in] value New value as a string
 *
 * @return 0 Successfully sent to HAB associated with the resource
 * @return -1 Failed to send to the HAB associated with the resource
 *
 * @note A return value of 0 does NOT mean that the Resource was actually
 * updated on the Node, just that the HAB accepted the request.
 */
int bionet_set_resource(const bionet_resource_t *resource, const char *value);


/**
 * @brief  Sends a message to Bionet requesting that the specified
 * Resource be set to the specified value.
 *
 * @param[in] hab_type string of the form "<HAB-Type>" or the wildcard "*"
 * @param[in] hab_id string of the form "<HAB-ID>" or the wildcard "*"
 * @param[in] node_id string of the form "<Node-ID>" or the wildcard "*"
 * @param[in] resource_id string of the form "<Resource-ID>" or the wildcard "*"
 * @param[in] value New value as a string
 *
 * @return 0 Successfully sent to HAB associated with the resource
 * @return -1 Failed to send to the HAB associated with the resource
 *
 * @note A return value of 0 does NOT mean that the Resource was actually
 * updated on the Node, just that the HAB accepted the request.
 */
int bionet_set_resource_by_habtype_habid_nodeid_resourceid(
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    const char *value);


/**
 * @brief  Sends a message to Bionet requesting that the specified
 * Resource be set to the specified value.
 *
 * @param[in] resource_name_pattern A string in the form 
 *"<HAB-Type>.<HAB-ID>.<Node-ID>:<Resource-ID>" where any component may be
 * the wildcard "*".
 * @param[in] value New value as a string
 *
 * @return 0 Successfully sent to HAB associated with the resource
 * @return -1 Failed to send to the HAB associated with the resource
 *
 * @note A return value of 0 does NOT mean that the Resource was actually
 * updated on the Node, just that the HAB accepted the request.
 */
int bionet_set_resource_by_name_pattern(const char *resource_name_pattern, const char *value);


#if 0

/**
 * @brief Returns all matching HABs the NAG knows about.
 *
 * @param[out] habs Pointer to the GSList to receive the list of HABs.  This
 * list will have an entry for each matching HAB Bionet knows about. The data
 * of each entry is a bionet_hab_t pointer. The list will be NULL if no
 * matching HABs are known.
 * @param[in] hab_name_pattern String of the form "<HAB-Type>.<HAB-ID>", where
 * either or both of HAB-Type and HAB-ID may be the wildcard "*".
 *
 * @return 0 Success (*habs is valid, possibly NULL)
 * @return -1 Error (*habs is undefined)
 *
 * @note Not yet implemented
 */
int bionet_list_habs_by_name_pattern(GSList **habs, const char *hab_name_pattern);


/**
 * @brief Returns all matching HABs the NAG knows about.
 *
 * @param[out] habs Pointer to the GSList to receive the list of HABs.  This
 * list will have an entry for each matching HAB Bionet knows about. The data
 * of each entry is a bionet_hab_t pointer. The list will be NULL if no
 * matching HABs are known.
 * @param[in] hab_type HAB-Type string or the wildcard "*"
 * @param[in] hab_id HAB-ID string or the wildcard "*"
 *
 * @return 0 Success (*habs is valid, possibly NULL)
 * @return -1 Error (*habs is undefined)
 *
 * @note Not yet implemented
 */
int bionet_list_habs_by_type_and_id(GSList **habs, const char *hab_type, const char *hab_id);


/**
 * @brief Returns all matching HABs the NAG knows about.
 *
 * @param[out] habs Pointer to the GSList to receive the list of HABs.  This
 * list will have an entry for each matching HAB Bionet knows about. The data
 * of each entry is a bionet_hab_t pointer. The list will be NULL if no
 * matching HABs are known.
 *
 * @return 0 Success (*habs is valid, possibly NULL)
 * @return -1 Error (*habs is undefined)
 *
 * @note Not yet implemented
 */
int bionet_list_all_habs(GSList **habs);


/**
 * @brief Gets the list of Nodes that Bionet knows about.  
 *
 * @param[out] nodes Pointer to the GSList to receive the list of Nodes. This
 * list will have an entry for each matching Node Bionet knows about. The data
 * of each entry is a bionet_node_t pointer. The list will be NULL if no
 * matching Nodes are known.
 * @param[in] node_name_pattern String of the form
 * "<HAB-Type>.<HAB-ID>.<Node-ID>" where any component may be the wildcard "*".
 *
 * @return 0 Success (*nodes is valid, possibly NULL)
 * @return -1 Error (*nodes is undefined).
 *
 * @note Not yet implemented
 */
int bionet_list_nodes_by_name_pattern(GSList **nodes, const char *node_name_pattern);


/**
 * @brief Gets the list of Nodes that Bionet knows about.  
 *
 * @param[out] nodes Pointer to the GSList to receive the list of Nodes. This
 * list will have an entry for each matching Node Bionet knows about. The data
 * of each entry is a bionet_node_t pointer. The list will be NULL if no
 * matching Nodes are known.
 * @param[in] hab_type HAB-Type string or the wildcard "*"
 * @param[in] hab_id HAB-ID string or the wildcard "*"
 * @param[in] node_id Node-ID string or the wildcard "*"
 *
 * @return 0 Success (*nodes is valid, possibly NULL)
 * @return -1 Error (*nodes is undefined).
 *
 * @note Not yet implemented
 */
int bionet_list_nodes_by_habtype_habid_nodeid(GSList **nodes, const char *hab_type,  const char *hab_id, const char *node_id);


/**
 * @brief Gets the list of Nodes that Bionet knows about.  
 *
 * @param[out] nodes Pointer to the GSList to receive the list of Nodes. This
 * list will have an entry for each matching Node Bionet knows about. The data
 * of each entry is a bionet_node_t pointer. The list will be NULL if no
 * matching Nodes are known.
 *
 * @return 0 Success (*nodes is valid, possibly NULL)
 * @return -1 Error (*nodes is undefined).
 *
 * @note Not yet implemented
 */
int bionet_list_all_nodes(GSList **nodes);

#endif /* 0 */


#endif // __BIONET_H


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
