
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


/**
 * @file hardware-abstractor.h
 * Describes the Bionet Hardware-Abstractor (HAB) API.
 */


#ifndef __HARDWARE_ABSTRACTOR_H
#define __HARDWARE_ABSTRACTOR_H


#include "bionet-util.h"


/**
 * @brief Connects to Bionet.  
 *
 * This function must be called before any other HAB library functions.
 *
 * @param[in] hab Pointer to HAB structure to represent this process to
 * the Bionet network.
 *              
 * @note The HAB-Type and HAB-ID may be set to indicate the name that the HAB
 * will use on the Bionet network. If unspecified the HAB library will update
 * the HAB structure with the program name for HAB-Type and hostname for HAB-ID
 *
 * @note The HAB library makes a copy of the passed-in HAB, and reads from it
 * as needed to satisfy subscription requests from Clients. The HAB library 
 * never modifies the HAB (except for possibly setting the HAB-Type and HAB-ID,
 * if the user choses not to).
 *
 * @return The Bionet file descriptor on success
 * @return -1 on failure.
 *              
 * @note The Bionet file descriptor must not be read or written directly by
 * the HAB. Only the HAB library may read and write it. The HAB should call 
 * hab_read() whenever the file descriptor is readable, or if it wants to
 * poll it.
 */
int hab_connect(bionet_hab_t *hab);


/**
 * @brief Called to report the addition of a new Node to the network.
 *
 * @param[in] node The Node to report.
 *
 * @return 0 Success
 * @return -1 Failure
 *
 * @note The Node passed in must have been added to the HAB before calling this
 * function. The Node must remain part of the HAB until the user is ready to
 * call hab_report_lost_node()
 */
int hab_report_new_node(const bionet_node_t *node);


/**
 * @brief Called to report that the value of some Resources have changed.
 *
 * Only Resources that have changed are reported.
 *
 * @param[in] node The Node to report.
 *
 * @return 0 Success
 * @return -1 Failure
 */
int hab_report_datapoints(const bionet_node_t *node);


/**
 * @brief Called to report that a Node has been dropped from the network.
 *
 * The node_id string passed in must correspond to a node that has been
 * removed from the HAB.  It may be freed or overwritten after the
 * function returns.
 *
 * @param[in] node_id The Node ID string.
 *
 * @return 0 Success
 * @return -1 Failure
 */
int hab_report_lost_node(const char *node_id);


/**
 * @brief read any pending messages and if appropriate call callback function.  
 * 
 * The HAB should call this function when the Bionet file descriptor returned
 * from hab_connect() is readable, or when the HAB wants to poll the fd.  
 *
 * @note See hab_register_callback_set_resource().
 */
void hab_read(void);


/**
 * @brief Registers the HAB's Set-Resource callback function.
 *
 * The Hardware Abstractor library will call the callback whenever
 * a Bionet Client sends a Set-Resource command.
 *
 * The callback function is responsible for getting the commanded value out
 * to the physical node, and (if appropriate here) updating the Resource
 * value and timestamp, and calling hab_report_resource_update().
 *
 * @param cb_set_resource  The new callback function.
 *
 * The callback function gets two arguments: the Resource that is being
 * commanded, and the new value.
 *
 * The Resource is one that the HAB application has previously reported
 * with hab_report_new_node(), and that has not been removed with
 * hab_report_lost_node().
 *
 * The value is the new value requested by the Client.  It is of the
 * appropriate data type for the Resource.  It will be freed by the HAB
 * library when the callback returns.
 */

void hab_register_callback_set_resource(void (*cb_set_resource)(bionet_resource_t *resource, const bionet_datapoint_value_t *value));




#endif // __HARDWARE_ABSTRACTOR_H



// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
