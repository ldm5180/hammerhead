
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
 * @retval >= 0 Success
 * @retval -1 Failure
 *              
 * @note The Bionet file descriptor must not be read or written directly by
 * the HAB. Only the HAB library may read and write it. The HAB should call 
 * hab_read() whenever the file descriptor is readable, or if it wants to
 * poll it.
 */
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
 * @retval 0 Success
 * @retval -1 Failure
 */
int hab_report_lost_node(const char *node_id);


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
int hab_publish_stream(const bionet_stream_t *stream, const void *buffer, size_t count);


/**
 * @brief Read and handle any pending Bionet events.
 * 
 * The HAB should call this function whenever the Bionet file descriptor
 * returned from hab_connect() becomes readable.
 *
 * @see hab_register_callback_set_resource().
 */
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
void hab_register_callback_stream_unsubscription(void (*cb_stream_unsubscription)(const char *client_id, const bionet_stream_t *stream));




#endif // __HARDWARE_ABSTRACTOR_H



// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
