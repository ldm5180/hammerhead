
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


/**
 * @file bionet-stream.h
 * Functions for dealing with Bionet Streams (such as they are).
 * FIXME: bionet streams needs some serious Tender Loving Care
 */


#ifndef __BIONET_STREAM_H
#define __BIONET_STREAM_H


#include <stdint.h>




typedef enum {
    BIONET_STREAM_DIRECTION_INVALID = -1,
    BIONET_STREAM_DIRECTION_PRODUCER,
    BIONET_STREAM_DIRECTION_CONSUMER
} bionet_stream_direction_t;

#define  BIONET_STREAM_DIRECTION_MIN  BIONET_STREAM_DIRECTION_PRODUCER
#define  BIONET_STREAM_DIRECTION_MAX  BIONET_STREAM_DIRECTION_CONSUMER




struct bionet_stream {
    const bionet_node_t *node;

    // this describes the stream
    char *id;
    bionet_stream_direction_t direction;
    char *type;

    void *user_data;
};




/**
 * @brief Allocates and initializes a new Stream.
 *
 * @param node The Node that this Stream belongs to, or NULL if there isn't
 *            a Node.
 * @param id The ID of this Stream.
 * @param direction The direction of the new Stream.
 * @param type The type of the Stream (currently only "audio" is supported).
 * @param host The host that the Stream is available on (optional, defaults to
 *             the local host).
 * @param port The port that the Stream is available on.
 *
 * @return Pointer to the new Stream.
 * @retval NULL Error
 * @retval >0 Success
 */
bionet_stream_t *bionet_stream_new(
    const bionet_node_t *node,
    const char *id,
    bionet_stream_direction_t direction,
    const char *type
);


/**
 * @brief Allocates and initializes a new Stream.
 *
 * @param node The Node that this Stream belongs to, or NULL if there isn't
 *            a Node.
 * @param id The ID of this Stream.
 * @param direction_str The direction of the new Stream.
 * @param type The type of the Stream (currently only "audio" is supported).
 * @param host The host that the Stream is available on (optional, defaults to
 *             the local host).
 * @param port_str The port that the Stream is available on.
 *
 * @return Pointer to the new Stream.
 * @retval NULL Error
 * @retval >0 Success
 */
bionet_stream_t *bionet_stream_new_from_strings(
    const bionet_node_t *node,
    const char *id,
    const char *direction_str,
    const char *type
);


/**
 * @brief Binds the Stream to an ephemeral (random, unused) port and
 *        starts listening.  Sets the Stream's "port" member to
 *        whatever port it got.
 *
 * @param[in] stream The Stream to listen on.
 *
 * @return The listening socket file descriptor
 * @retval >=0 Success
 * @retval -1 Failure
 */
int bionet_stream_listen(bionet_stream_t *stream);


/**
 * @brief Accepts a connection on a listening Stream.
 *
 * @param[in] stream The Stream to accept on
 * @param[in] listening_socket The listening socket file descriptor.
 *
 * @return The new socket file descriptor
 * @retval >=0 Success
 * @retval -1 Failure
 */
int bionet_stream_accept(bionet_stream_t *stream, int listening_socket);


/**
 * @brief Makes a connection to a Stream
 *
 * @param[in] stream The Stream to connect to.
 *
 * @return The new socket file descriptor
 * @retval >=0 Success
 * @retval -1 Failure
 */
int bionet_stream_connect(bionet_stream_t *stream);

*/

/**
 * @brief Free a Stream
 *
 * The user_data field of the stream structure must be NULL on entry to this
 * function, or it will leak memory.  All other dynamically allocated fields
 * of the stream structure are freed (id, type, host, hab_type, hab_id, 
 * and node_id).
 *
 * @param[in] stream The Stream to free.
 */
void bionet_stream_free(bionet_stream_t *stream);


/**
 * @brief Convert a text string describing a Stream direction into
 *        the appropriate bionet_stream_direction_t.
 *
 * @param[in] direction_string The string to convert.
 *
 * @return The bionet_stream_direction_t.
 */
bionet_stream_direction_t bionet_stream_direction_from_string(const char *direction_string);


/**
 * @brief Converts a bionet_stream_direction_t into a descriptive text string.
 *
 * @param[in] direction The bionet_stream_direction_t to convert.
 *
 * @return The string
 * @retval >0 Success
 * @retval NULL Failure
 */
const char *bionet_stream_direction_to_string(bionet_stream_direction_t direction);




#endif // __BIONET_STREAM_H

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
