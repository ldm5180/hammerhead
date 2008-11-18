
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


//
// This header file describes the Bionet Hardware-Abstractor (HAB) API.
//


#ifndef __HARDWARE_ABSTRACTOR_H
#define __HARDWARE_ABSTRACTOR_H




#include "bionet-util.h"




//
//
//       NAME:  hab_connect()
//
//
//   FUNCTION:  Connects to Bionet.  This function must be called before
//              any other HAB library functions.
//
//
//  ARGUMENTS:  A HAB structure to represent this process to the Bionet
//              network.
//              
//              The HAB-Type and HAB-ID may be set to indicate the name
//              that the HAB will use on the Bionet network.
//              If unspecified the HAB library will update the HAB
//              structure with the program name for HAB-Type and the
//              hostname for HAB-ID.
//
//              The HAB library makes a copy of the passed-in HAB, and
//              reads from it as needed to satisfy subscription requests
//              from Clients.  The HAB library never modifies the HAB
//              (except for possibly setting the HAB-Type and HAB-ID, if
//              the user chose not to).
//
//
//    RETURNS:  The Bionet file descriptor on success, -1 on failure.
//              Note: The Bionet file descriptor must not be read or
//              written directly by the HAB.  Only the HAB library may read
//              and write it.  The HAB may select on the file descriptor,
//              when it's readable the HAB should call hab_read().
//
//

int hab_connect(bionet_hab_t *hab);




//
//
//       NAME:  hab_report_new_node()
//
//
//   FUNCTION:  Called to report the addition of a new Node to the network.
//
//              The Node passed in must have been added to the HAB before
//              calling this function.  The Node must remain part of the
//              HAB until the user is ready to call hab_report_lost_node()
//              (see below).
//
//
//  ARGUMENTS:  The Node to report.
//
//
//    RETURNS:  0 on success, -1 on failure.
//
//

int hab_report_new_node(const bionet_node_t *node);




//
//
//       NAME:  hab_report_datapoints()
//
//
//   FUNCTION:  Called to report that the value of some Resources have
//              changed.  Only Resources that have changed are reported.
//
//
//  ARGUMENTS:  The Node to report.
//
//
//    RETURNS:  0 on success, -1 on failure.
//
//

int hab_report_datapoints(const bionet_node_t *node);




//
//
//       NAME:  hab_report_lost_node()
//
//
//   FUNCTION:  Called to report that a Node has been dropped from the
//              network.
//
//              The node_id string passed in must correspond to a node that
//              has been removed from the HAB.  It may be freed or
//              overwritten after the function returns.
//
//
//  ARGUMENTS:  The Node ID string.
//
//
//    RETURNS:  0 on success, -1 on failure.
//
//

int hab_report_lost_node(const char *node_id);




//
//
//       NAME:  hab_read()
//
//   FUNCTION:  When the Bionet file descriptor returned from hab_connect()
//              is readable, the HAB should call this function.  It will
//              read any pending messages and call the appropriate
//              registered callback function.
//              See hab_register_callback_set_resource().
//
//  ARGUMENTS:  None.
//
//    RETURNS:  Nothing.
//
//

void hab_read(void);




//
//
//       NAME:  hab_register_callback_set_resource()
//
//   FUNCTION:  Registers the callback function with the Hardware
//              Abstractor library.  This function will be called when
//              a Bionet Client sends a set-resource command.
//
//              The callback function is responsible for getting the
//              commanded value out to the physical node, and (if
//              appropriate here) updating the Resource value and
//              timestamp, and calling hab_report_resource_update().
//
//  ARGUMENTS:  The new callback function.
//
//    RETURNS:  Nothing.
//
//

void hab_register_callback_set_resource(void (*cb_set_resource)(const char *node_id, const char *resource_id, const char *value));




#if 0

//
//
//       NAME:  hab_set_type()
//              hab_set_id()
//
//
//   FUNCTION:  The Network Aggregator refers to Hardware Abstractors by
//              Type and ID.  While there may be several HABs of the same
//              Type, the concatenation of Type and ID must be globally
//              unique.
//
//              These functions may be called by the HAB before connecting
//              to the NAG, ie before calling hab_connect_to_nag() or
//              calling any function that needs to communicate with the NAG.
//
//              If these functions are not called before connecting to the
//              NAG, Type will default to the program name, and ID will
//              default to the computer's hostname.
//
//              The strings that are passed in are copied into private
//              memory within the HAB library and may be freed or
//              overwritten after the functions return.
//
//
//  ARGUMENTS:  The new HAB Type or HAB ID strings.
//
//
//    RETURNS:  On success it returns 0.  On failure it returns -1 and sets
//              errno:
//
//                  EINVAL: The input string contained an invalid
//                      character.
//
//                  ENOMEM: Out of memory.
//
//

int hab_set_type(const char *new_hab_type);
int hab_set_id(const char *new_hab_id);




//
//
//       NAME:  hab_set_nag_hostname()
//
//   FUNCTION:  Tells the HAB library what host to connect to the NAG on.
//              The default is "localhost".
//
//  ARGUMENTS:  The name of the host that the NAG is running on.
//
//    RETURNS:  Nothing.
//
//

void hab_set_nag_hostname(const char *hostname);




//
//
//       NAME:  hab_disconnect_from_nag()
//
//
//   FUNCTION:  Closes the connection to the NAG.  Calling this function is
//              optional.
//
//
//  ARGUMENTS:  None.
//
//
//    RETURNS:  Nothing.
//
//

void hab_disconnect_from_nag(void);




//
//    
//       NAME:  hab_is_connected()
//
//   FUNCTION:  Informs caller if the HAB is connected to the NAG or not.
//
//  ARGUMENTS:  None.
//
//    RETURNS:  Returns false (0) if the HAB is _not_ connected to the NAG,
//              returns true (non-zero) if the HAB _is_ connected to the
//              NAG.
//
//

int hab_is_connected(void);




//
//
//       NAME:  hab_get_nag_error()
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

const char *hab_get_nag_error(void);




//
//
//       NAME:  hab_handle_queued_nag_messages()
//
//   FUNCTION:  The NAG may send messages containing set-resource requests
//              to the HAB at any time.  If these messages arrive while the
//              HAB is waiting for a reply to a message it has sent, the
//              library will queue them for later processing.  This
//              function takes the messages from the queue and processes
//              them by calling the appropriate registered callback.
//
//              The HAB should call this function when returning from
//              library calls.
//
//              (A future version of the Bionet Hardware Abstractor library
//              may call this automatically before returning to the
//              caller.)
//
//  ARGUMENTS:  None.
//
//    RETURNS:  Nothing.
//
//

void hab_handle_queued_nag_messages(void);

#endif




#endif // __HARDWARE_ABSTRACTOR_H


