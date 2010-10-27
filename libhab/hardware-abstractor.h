
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

/**
 * @file hardware-abstractor.h
 * Describes the Bionet Hardware-Abstractor (HAB) API.
 */


#ifndef __HARDWARE_ABSTRACTOR_H
#define __HARDWARE_ABSTRACTOR_H

#include "libhab-decl.h"
#include "bionet-util.h"


#ifdef __cplusplus
extern "C" {
#endif

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
 * @retval >= 0 Success
 * @retval -1 Failure
 *              
 * @note The Bionet file descriptor must not be read or written directly by
 * the HAB. Only the HAB library may read and write it. The HAB should call 
 * hab_read() whenever the file descriptor is readable, or if it wants to
 * poll it.
 */
LIBHAB_API_DECL 
int hab_connect(bionet_hab_t *hab);


/**
 * @brief Disconnects from Bionet.  
 *
 * This function may be called when the HAB has completed all its Bionet
 * communications and is ready to terminate.  It ensures that all published
 * information makes it out to Bionet before returning.
 *
 * @note This function will *not* publish anything new (for example, Nodes
 * added to the HAB but not reported by hab_report_new_node()), it just
 * ensures that already-published information is actually sent to the
 * subscribers.
 */
LIBHAB_API_DECL 
void hab_disconnect(void);


/**
 * @brief Called to report the addition of a new Node to the network.
 *
 * @param[in] node The Node to report.
 *
 * @retval 0 Success
 * @retval -1 Failure
 *
 * @note The Node passed in must have been added to the HAB before calling this
 * function. The Node must remain part of the HAB until the user is ready to
 * call hab_report_lost_node()
 */
LIBHAB_API_DECL 
int hab_report_new_node(const bionet_node_t *node);


/**
 * @brief Called to report that the value of some Resources have changed.
 *
 * Only Resources that have changed are reported.
 *
 * @param[in] node The Node to report.
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
LIBHAB_API_DECL 
int hab_report_datapoints(const bionet_node_t *node);


/**
 * @brief Called to report that a Node has been dropped from the network.
 *
 * The node_id string passed in must correspond to a node that has been
 * removed from the HAB.  It may be freed or overwritten after the
 * function returns.
 *
 * @param[in] node The Node.
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
LIBHAB_API_DECL 
int hab_report_lost_node(const bionet_node_t * node);


/**
 * @brief Publishes a block of data to a Stream.
 *
 * @param[in] stream The Stream to publish on.
 * @param[in] buffer The data to publish.
 * @param[in] count The number of bytes in buf.
 *
 * @retval 0 Success
 * @retval -1 Failure
 */
LIBHAB_API_DECL 
int hab_publish_stream(const bionet_stream_t *stream, const void *buffer, size_t count);


/**
 * @brief Read and handle any pending Bionet events.
 * 
 * The HAB should call this function whenever the Bionet file descriptor
 * returned from hab_connect() becomes readable.
 *
 * @see hab_register_callback_set_resource().
 */
LIBHAB_API_DECL 
void hab_read(void);


/**
 * @brief Read and handle any pending Bionet events and timeout if there 
 * is nothing to read.
 * 
 * The HAB should call this function whenever the Bionet file descriptor
 * returned from hab_connect() becomes readable.
 * 
 * @param[in] timeout Timeout length or NULL to block.
 *
 * @retval 0 Success
 * @retval -1 Failure
 *
 * @see hab_register_callback_set_resource()
 */
LIBHAB_API_DECL 
int hab_read_with_timeout(struct timeval *timeout);


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
LIBHAB_API_DECL 
void hab_register_callback_set_resource(void (*cb_set_resource)(bionet_resource_t *resource, bionet_value_t *value));


/**
 * @brief Registers the HAB's Stream-Data callback function.
 *
 * The Hardware Abstractor library will call this callback whenever
 * a Bionet Client sends some data for one of the HAB's Consumer Streams.
 *
 * @param cb_set_resource The new callback function.
 *
 * The callback function gets four arguments: the Client that's sending the
 * data, the Stream that's being written to, and the data being written.
 */
LIBHAB_API_DECL 
void hab_register_callback_stream_data(void (*cb_set_resource)(const char *client_id, bionet_stream_t *stream, const void *buf, unsigned int size));


/**
 * @brief Registers the HAB's Lost-Client callback function.
 *
 * The Hardware Abstractor library will call the callback whenever
 * a Bionet Client disconnects.
 *
 * @param cb_lost_client  The new callback function.
 *
 * The callback function gets a single argument: the unique "id" of the
 * disconnecting Client.
 */
LIBHAB_API_DECL 
void hab_register_callback_lost_client(void (*cb_lost_client)(const char *client_id));


/**
 * @brief Registers the HAB's Stream-subscription callback function.
 *
 * The Hardware Abstractor library will call this callback whenever
 * a Bionet Client subscribes to a Stream.
 *
 * @param cb_stream_subscription  The new callback function.
 *
 * The callback function gets two arguments, the Client's unique "id", and
 * the Stream being subscribed to.
 */
LIBHAB_API_DECL 
void hab_register_callback_stream_subscription(void (*cb_stream_subscription)(const char *client_id, const bionet_stream_t *stream));


/**
 * @brief Registers the HAB's Stream-Unsubscription callback function.
 *
 * The Hardware Abstractor library will call this callback whenever
 * a Bionet Client unsubscribes from a Stream.
 *
 * @param cb_stream_unsubscription  The new callback function.
 *
 * The callback function gets two arguments, the Client's unique "id", and
 * the Stream being unsubscribed from.
 */
LIBHAB_API_DECL 
void hab_register_callback_stream_unsubscription(void (*cb_stream_unsubscription)(const char *client_id, const bionet_stream_t *stream));


/**
 * @brief Initialize security data structures for the HAB.
 *
 * @param[in] dir Directory containing keys, certificates,
 * and other security configuration data.
 * @param[in] require 0 for optional security, 1 for required security
 * 
 * @return 0 Success
 * @return -1 Failure
 *
 * @note Security must be initialized prior to call hab_connect()
 */
LIBHAB_API_DECL 
int hab_init_security(const char * dir, int require);


/**
 * @brief Publish the Bionet version information
 * 
 * @param[in] hab Pointer to HAB structure to represent this process to
 * the Bionet network.
 * @param[in] flags Union of flags of Bionet info to publish
 *
 */
#define BIONET_INFO_VERSION_FLAG 0x00000001
LIBHAB_API_DECL 
int hab_publish_info(bionet_hab_t * hab, const uint32_t flags);


/**
 * @brief Persist the value of any datapoint set to this resource
 *
 * Using local storage, make the value of the datapoint persistent. This allows
 * the resource to have the same value next time the HAB is started.
 *
 * @param[in] resource The Resource
 *
 * @retval 0 Success. Resource's value will be persisted each time a datapoint
 * is published for this resource.
 * @retval 1 Failure. This resource's value will not be persisted.
 *
 * @note If the resource does not yet have a datapoint, after calling this function
 * it will have a datapoint containing the current time and the last value persisted
 * by this resource if it has ever been persisted before.
 *
 * @note This function does not do the persisting. It only marks the resource as a
 * resource which requires persistence. Persisting is done as part of publishing using
 * hab_report_datapoints() or hab_report_new_node()
 *
 * @note If a persistance directory other than the default is desired, 
 * hab_set_persist_directory() must be called before this function.
 */
int hab_persist_resource(bionet_resource_t * resource);

/**
 * @brief Set the directory to which persistency data is written
 *
 * The default directory on POSIX systems is /var/lib/bionet
 *
 * @param[in] dir Directory to use
 *
 * @retval 0 Success.
 * @retval 1 Failure.
 *
 * @note This needs to be called before any calls to hab_persist_resource()
 * unless the default is going to be used.
 */
LIBHAB_API_DECL
int hab_set_persist_directory(char * dir);


#ifdef __cplusplus
}
#endif

#endif // __HARDWARE_ABSTRACTOR_H



// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
