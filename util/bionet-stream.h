
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

#include "bionet-util.h"




typedef enum {
    BIONET_STREAM_DIRECTION_INVALID = -1,
    BIONET_STREAM_DIRECTION_PRODUCER,
    BIONET_STREAM_DIRECTION_CONSUMER
} bionet_stream_direction_t;

#define  BIONET_STREAM_DIRECTION_MIN  BIONET_STREAM_DIRECTION_PRODUCER
#define  BIONET_STREAM_DIRECTION_MAX  BIONET_STREAM_DIRECTION_CONSUMER




/**
 * @brief Allocates and initializes a new Stream.
 *
 * @param[in] node The Node that this Stream belongs to, or NULL if there isn't
 *            a Node.
 * @param[in] id The ID of this Stream.
 * @param[in] direction The direction of the new Stream.
 * @param[in] type The type of the Stream (a free-form ascii string).
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
 * @brief Get the ID of an existing Stream
 *
 * @param[in] stream Pointer to a Stream
 * 
 * @return Stream-ID string
 * @return NULL on failure
 *
 * @note Do not free the returned pointer
 */
const char * bionet_stream_get_id(const bionet_stream_t *stream);


/**
 * @brief Get the type of an existing Stream
 *
 * @param[in] stream Pointer to a Stream
 * 
 * @return Stream type string
 * @return NULL on failure
 * 
 * @note Do not free the returned pointer
 * @todo implement me
 */
const char * bionet_stream_get_type(const bionet_stream_t *stream);


/**
 * @brief Get the parent HAB of a Stream
 *
 * @param[in] stream Pointer to a Stream
 * 
 * @return Pointer to the Stream's HAB
 * @return NULL on failure
 */
bionet_hab_t * bionet_stream_get_hab(const bionet_stream_t *stream);


/**
 * @brief Get the parent Node of a Stream
 *
 * @param[in] stream Pointer to a Stream
 * 
 * @return Pointer to the Stream's Node
 * @return NULL on failure
 */
bionet_node_t * bionet_stream_get_node(const bionet_stream_t *stream);


/**
 * @brief Free a Stream
 *
 * The user_data field of the stream structure must be NULL on entry to this
 * function, or it will leak memory.  All other dynamically allocated fields
 * of the stream structure are freed.
 *
 * @param[in] stream The Stream to free.
 */
void bionet_stream_free(bionet_stream_t *stream);


/**
 * @brief Get the direction of a Stream
 *
 * @param[in] stream Pointer to a Stream
 *
 * @retval #BIONET_STREAM_DIRECTION_INVALID Failure
 * @retval #BIONET_STREAM_DIRECTION_PRODUCER Producer
 * @retval #BIONET_STREAM_DIRECTION_CONSUMER Consumer
 */
bionet_stream_direction_t bionet_stream_get_direction(const bionet_stream_t * stream);


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


/**
 * @brief Get the Bionet qualified name of the Stream
 *
 * Stream name is of the format <HAB-Type>.<HAB-ID>.<Node-ID>:<Stream-ID>
 *
 * @param[in] stream The Stream
 *
 * @return The name of the Stream, or NULL on error.
 */
const char *bionet_stream_get_name(const bionet_stream_t *stream);


/**
 * @brief Get the local-to-the-HAB name of a Stream
 *
 * The local Stream name is of the format <Node-ID>:<Stream-ID>
 *
 * @param[in] stream The Stream
 *
 * @return The local name of the Stream, or NULL on error.
 */
const char *bionet_stream_get_local_name(const bionet_stream_t *stream);


/**
 * @brief Set the user-data annotation of a Stream
 *
 * @param[in] stream The Stream
 * @param[in] user_data The data to annotate the stream with.
 */
void bionet_stream_set_user_data(bionet_stream_t *stream, const void *user_data);


/**
 * @brief Get the user-data annotation of a Stream
 *
 * @param[in] stream The Stream
 *
 * @return The user_data pointer, or NULL if none has been set.
 */
void *bionet_stream_get_user_data(const bionet_stream_t *stream);




#endif // __BIONET_STREAM_H

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
