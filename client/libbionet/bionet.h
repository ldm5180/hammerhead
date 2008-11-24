
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




//
//
//       NAME:  bionet_set_id()
//
//
//   FUNCTION:  The Client ID is used by the Network Aggregator when
//              logging information.  It exists only for convenience.
//
//              These functions may be called by the Client before
//              connecting to the NAG, ie before calling
//              bionet_connect_to_nag() and before calling any function
//              that needs to communicate with the NAG.
//
//              If this function is not called before connecting to the
//              NAG, the default value of 'user@host:port (program-name
//              [pid])' will be used.
//
//              The string passed in is copied into private memory within
//              the Bionet library and may be freed or overwritten after
//              the function returns.
//
//
//  ARGUMENTS:  The new Client ID string.
//
//
//    RETURNS:  0 on success, -1 on failure.
//
//

int bionet_set_id(const char *new_id);




//
//
//       NAME:  bionet_register_callback_new_hab()
//              bionet_register_callback_lost_hab()
//
//              bionet_register_callback_new_node()
//              bionet_register_callback_lost_node()
//
//              bionet_register_callback_datapoint()
//
//
//   FUNCTION:  Registers callback functions with the Bionet library.
//              These functions will be called when information is
//              published that the Client has subscribed to.
//
//              The 'cb_new_hab' and 'cb_lost_hab' functions get called
//              when a HAB that matches a HAB-list subscription joins and
//              leaves (respectively) the network.  HAB-list subscriptions
//              are created with the bionet_subscribe_hab_list() function
//              declared below.  The bionet_hab_t argument is the property
//              of the Bionet Client library, and must not be deallocated
//              by the callback functions.  The new-hab callback function
//              may set the bionet_hab_t's 'user_data' member.  The lost-hab
//              callback MUST properly unset and deallocate the 'user_data'
//              member or the application will leak memory.
//
//              The 'cb_new_node' and 'cb_lost_node' functions get called
//              when a Node that matches a Node-list subscription joins and
//              leaves (respectively) a HAB.  Node-list subscriptions are
//              created with the bionet_subscribe_node_list() function
//              declared below.  The bionet_node_t argument is the property
//              of the Bionet Client library, and must not be deallocated
//              by the callback functions.  The new-node callback may set
//              the bionet_node_t's 'user_data' member.  The lost-node
//              callback MUST properly unset and deallocate the 'user_data'
//              member or the application will leak memory.
//
//              The 'cb_datapoint' function gets called when a Resource
//              matching a Datapoint subscription gets a new value.
//              Datapoint subscriptions are created with the
//              bionet_subscribe_datapoint() function declared below.
//              The bionet_datapoint_t argument to the callback function is
//              the property of the Bionet Client library, and MUST not be
//              deallocated by the callback function.
//
//
//  ARGUMENTS:  The new callback function.
//
//
//    RETURNS:  Nothing.
//
//

void bionet_register_callback_new_hab(void (*cb_new_hab)(bionet_hab_t *hab));
void bionet_register_callback_lost_hab(void (*cb_lost_hab)(bionet_hab_t *hab));

void bionet_register_callback_new_node(void (*cb_new_node)(bionet_node_t *node));
void bionet_register_callback_lost_node(void (*cb_lost_node)(bionet_node_t *node));

void bionet_register_callback_datapoint(void (*cb_datapoint)(bionet_datapoint_t *datapoint));




//
//
//       NAME:  bionet_connect()
//
//
//   FUNCTION:  Connects to the Bionet network.  Calling this function from
//              the Client is optional, it will be called implicitly when
//              needed by other Bionet library functions.
//
//              If the connection is already opened, the function does
//              nothing and just returns the file descriptor.
//
//
//  ARGUMENTS:  None.
//
//
//    RETURNS:  On success, returns a non-blocking file descriptor
//              associated with the Bionet network.  The file descriptor
//              should not be read or written directly by the Client.
//              If the file descriptor is readable, the Client should call
//              bionet_read() to service it.  Since the fd is non-blocking,
//              the client may also call bionet_read() in a polling way,
//              though this is less efficient than using select() or poll()
//              on the fd.
//
//              On failure, returns -1.
//
//

int bionet_connect(void);




//
//
//       NAME:  bionet_is_connected()
//
//   FUNCTION:  Checks to see if the Bionet library is connected to Bionet.
//
//  ARGUMENTS:  None.
//
//    RETURNS:  Returns True (non-zero) if the library IS connected to
//              Bionet, returns False (0) if the library is NOT connected
//              to Bionet.
//
//

int bionet_is_connected(void);




//
//
//       NAME:  bionet_read()
//
//   FUNCTION:  This function should be called whenever the Bionet file
//              descriptor returned from bionet_connect() is readable, or
//              if the Client application wants to poll the file descriptor.
//              The function will read any pending messages from Bionet and
//              if appropriate call the callback functions.
//
//  ARGUMENTS:  None.
//
//    RETURNS:  True (non-zero) on success, False (zero) on failure.
//
//

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


//
//       NAME:  bionet_cache_lookup_hab()
//
//   FUNCTION:  Looks through the locally cached information for a specific
//              HAB.
//
//  ARGUMENTS:  The HAB-Type and HAB-ID to look up.
//
//    RETURNS:  The HAB if found, NULL if it was not found.
//

bionet_hab_t *bionet_cache_lookup_hab(const char *hab_type, const char *hab_id);




//
//       NAME:  bionet_cache_lookup_node()
//
//   FUNCTION:  Looks through the locally cached information for a specific
//              Node.
//
//  ARGUMENTS:  The HAB-Type, HAB-ID, and Node-ID to look up.
//
//    RETURNS:  The Node if found, NULL if it was not found.
//

bionet_node_t *bionet_cache_lookup_node(const char *hab_type, const char *hab_id, const char *node_id);




//
//       NAME:  bionet_cache_lookup_resource()
//
//   FUNCTION:  Looks through the locally cached information for a specific
//              Resource.
//
//  ARGUMENTS:  The HAB-Type, HAB-ID, Node-ID, and Resource-ID to look up.
//
//    RETURNS:  The Resource if found, NULL if it was not found.
//

bionet_resource_t *bionet_cache_lookup_resource(const char *hab_type, const char *hab_id, const char *node_id, const char *resource_id);




//
//       NAME:  bionet_cache_lookup_stream()
//
//   FUNCTION:  Looks through the locally cached information for a specific
//              Stream.
//
//  ARGUMENTS:  The HAB-Type, HAB-ID, Node-ID, and Stream-ID to look up.
//
//    RETURNS:  The stream if found, NULL if it was not found.
//

bionet_stream_t *bionet_cache_lookup_stream(const char *hab_type, const char *hab_id, const char *node_id, const char *resource_id);




//
//       NAME: bionet_subscribe_hab_list()
//
//
//   FUNCTION:  Subscribes the client to the Nag's list of HABs matching
//              the specified pattern.  When HABs matching the specified
//              pattern join or leave the NAG, the NAG will report the
//              fact to the Client.
//
//
//  ARGUMENTS:  'hab_name' is a string of the form "<HAB-Type>.<HAB-ID>",
//              where any component may be the wildcard "*".
//
//              'hab_type' and 'hab_id' are strings containing the
//              individual components of the name, not joined by ".".
//              Again "*" is the wildcard.
//
//
//    RETURNS:  0 on success, -1 on error
//
//

int bionet_subscribe_hab_list_by_name(const char *hab_name);
int bionet_subscribe_hab_list_by_habtype_habid(const char *hab_type,  const char *hab_id);




//
//
//       NAME: bionet_subscribe_node_list()
//
//
//   FUNCTION:  Subscribes the client to the Nag's list of Nodes matching
//              the specified pattern.  When Nodes matching the specified
//              pattern join or leave a network, the NAG will report the
//              fact to the Client.
//
//
//  ARGUMENTS:  'node_name' is a string of the form
//              "<HAB-Type>.<HAB-ID>.<Node-ID>", where any component may be
//              the wildcard "*".
//
//              'hab_type', 'hab_id', and 'node_id' are strings containing
//              the individual components of the name, not joined by ".".
//              Again "*" is the wildcard.
//
//
//    RETURNS:  0 on success, -1 on error
//
//

int bionet_subscribe_node_list_by_name(const char *node_name);
int bionet_subscribe_node_list_by_habtype_habid_nodeid(const char *hab_type,  const char *hab_id, const char *node_id);




//
//
//       NAME: bionet_subscribe_datapoints()
//
//
//   FUNCTION:  Subscribes the client to the values of Resources matching
//              the specified pattern.  When Resource matching the
//              specified pattern changes, the NAG will report the
//              fact to the Client.
//
//
//  ARGUMENTS:  'resource_name' is a string of the form
//              "<HAB-Type>.<HAB-ID>.<Node-ID>:<Resource-ID>", where any
//              component may be the wildcard "*".
//
//              'hab_type', 'hab_id', 'node_id', and 'resource_id' are
//              strings containing the individual components of the name,
//              not joined by ".".  Again "*" is the wildcard.
//
//
//    RETURNS:  0 on success, -1 on error
//
//

int bionet_subscribe_datapoints_by_name(const char *resource_name);
int bionet_subscribe_datapoints_by_habtype_habid_nodeid_resourceid(const char *hab_type,  const char *hab_id, const char *node_id, const char *resource_id);




#if 0





//
//
//       NAME:  bionet_nag_message_queue_is_empty()
//
//   FUNCTION:  This function checks to see if the library has any queued
//              NAG messages.
//
//  ARGUMENTS:  None.
//
//    RETURNS:  TRUE (1) if the queue is empty, FALSE (0) if there are
//              messages in the queue.
//
//

int bionet_nag_message_queue_is_empty(void);




//
//
//       NAME:  bionet_read_from_nag_but_dont_handle_messages()
//
//   FUNCTION:  This function is just like bionet_read_from_nag(), except
//              that it does _not_ call bionet_handle_queued_nag_messages().
//              It just reads any pending messages and puts them in the
//              queue for later.
//
//  ARGUMENTS:  None.
//
//    RETURNS:  Nothing.
//
//

void bionet_read_from_nag_but_dont_handle_messages(void);




//
//
//       NAME:  bionet_handle_one_queued_nag_message()
//
//   FUNCTION:  The NAG may send messages containing subscribed information
//              to the Client at any time.  If these messages arrive while
//              the Client is waiting for a reply to a message it has sent,
//              the library will queue them for later processing.  This
//              function takes one message from the queue and processes
//              it by calling the appropriate registered callback.
//
//  ARGUMENTS:  None.
//
//    RETURNS:  Nothing.
//
//

void bionet_handle_one_queued_nag_message(void);




//
//
//       NAME:  bionet_handle_queued_nag_messages()
//
//   FUNCTION:  This function handles all queued message from the NAG, by
//              calling bionet_handle_one_queued_nag_message() repeatedly
//              until it returns -1.
//
//  ARGUMENTS:  None.
//
//    RETURNS:  Nothing.
//
//

void bionet_handle_queued_nag_messages(void);




//
//
//       NAME:  bionet_get_nag_error()
//
//   FUNCTION:  Returns the most recent error message from the NAG, if any.
//
//  ARGUMENTS:  None
//
//    RETURNS:  If the most recent request to the NAG resulted in an error,
//              the function returns a pointer to the error message string.
//              If the most recent request succeeded, or if no request has
//              been made, it returns NULL.
//
//              The error string is valid until the next NAG request is
//              made.  Do not free or modify the string.
//
//

const char *bionet_get_nag_error(void);




//
//
//       NAME:  bionet_list_habs_by_name_pattern()
//              bionet_list_habs_by_type_and_id()
//              bionet_list_all_habs()
//
//
//   FUNCTION:  Returns all matching HABs the NAG knows about.
//
//
//  ARGUMENTS:  'habs' is a pointer to the GSList to receive the list of
//              HABs.  This list will have an entry for each matching HAB
//              the NAG knows about.  The data of each entry is a
//              bionet_hab_t pointer.  The list will be NULL if no matching
//              HABs are known.
//
//              'hab_name_pattern' is a string of the form
//              "<HAB-Type>.<HAB-ID>", where either or both of HAB-Type and
//              HAB-ID may be "*".
//
//              'hab_type' and 'hab_id' is just the HAB-Type and HAB-ID
//              strings individually, not joined by a ".".
//
//
//    RETURNS:  0 on success (*habs is valid, possibly NULL), -1 on error
//              (*habs is undefined).
//
//

int bionet_list_habs_by_name_pattern(GSList **habs, const char *hab_name_pattern);
int bionet_list_habs_by_type_and_id(GSList **habs, const char *hab_type, const char *hab_id);
int bionet_list_all_habs(GSList **habs);




//
//
//       NAME:  bionet_list_nodes_by_name_pattern()
//              bionet_list_nodes_by_habtype_habid_nodeid()
//              bionet_list_all_nodes()
//
//
//   FUNCTION:  Gets the list of Nodes that the NAG knows about.  
//
//
//  ARGUMENTS:  'nodes' is a pointer to the GSList to receive the list of
//              Nodes.  This list will have an entry for each matching Node
//              the NAG knows about.  The data of each entry is a
//              bionet_node_t pointer.  The list will be NULL if no
//              matching Nodes are known.
//
//              'node_name_pattern' is a string of the form
//              "<HAB-Type>.<HAB-ID>.<Node-ID>", where any component may be
//              the wildcard "*".
//
//              'hab_type', 'hab_id', and 'node_id' are strings containing
//              the individual components of the name, not joined by ".".
//              Again "*" is the wildcard.
//
//
//    RETURNS:  0 on success (*nodes is valid, possibly NULL), -1 on error
//              (*nodes is undefined).
//
//

int bionet_list_nodes_by_name_pattern(GSList **nodes, const char *node_name_pattern);
int bionet_list_nodes_by_habtype_habid_nodeid(GSList **nodes, const char *hab_type,  const char *hab_id, const char *node_id);
int bionet_list_all_nodes(GSList **nodes);




//
//
//       NAME:  bionet_set_resource()
//              bionet_set_resource_by_habtype_habid_nodeid_resourceid()
//
//   FUNCTION:  Sends a message to the NAG requesting that the specified
//              Resource be set to the specified value.
//
//  ARGUMENTS:  The Resource to set, and it's new value (as a string).
//
//    RETURNS:  0 if the message was successfully sent to the NAG, -1 if it
//              was not.  Note that a return value of 0 does NOT mean that
//              the Resource was actually updated out on the Node, just
//              that the NAG accepted the request.
//
//

int bionet_set_resource(const bionet_resource_t *resource, const char *value);

int bionet_set_resource_by_habtype_habid_nodeid_resourceid(
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    const char *value
);

int bionet_set_resource_by_name_pattern(const char *resource_name_pattern, const char *value);


//
//       NAME:  bionet_read_from_string()
//
//   FUNCTION:  When the NAG socket returned from bionet_connect_to_nag()
//              is readable, the Client application can call this
//              function.  It will read any pending messages from the 
//              given message and call the appropriate registered 
//              callback functions (by calling 
//              bionet_handle_queued_nag_messages()).
//
//  ARGUMENTS:  None.
//
//    RETURNS:  The number of bytes that were read/used.
//
//
//
//
// int bionet_read_from_string(char* message, int length);
//
//
//       NAME:  bionet_read_from_string_but_dont_handle_messages()
//
//   FUNCTION:  This function is just like bionet_read_from_string(), except
//              that it does _not_ call bionet_handle_queued_nag_messages().
//              It just reads any pending messages and puts them in the
//              queue for later.
//
//  ARGUMENTS:  None.
//
//    RETURNS:  The number of bytes that were read/used.
//
//
// int bionet_read_from_string_but_dont_handle_messages(char* nxio_message, int length);

#endif


#endif // __BIONET_H


