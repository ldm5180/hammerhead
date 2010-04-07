
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


/**
 * @file bionet-stream.h
 * Functions for dealing with Bionet Streams (such as they are).
 * FIXME: bionet streams needs some serious Tender Loving Care
 */


#ifndef __BIONET_STREAM_H
#define __BIONET_STREAM_H


#include "libbionet-util-decl.h"

#ifdef __cplusplus
extern "C" {
#endif


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
BIONET_UTIL_API_DECL
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
BIONET_UTIL_API_DECL
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
BIONET_UTIL_API_DECL
const char * bionet_stream_get_type(const bionet_stream_t *stream);


/**
 * @brief Get the parent HAB of a Stream
 *
 * @param[in] stream Pointer to a Stream
 * 
 * @return Pointer to the Stream's HAB
 * @return NULL on failure
 *
 * @note This is shorthand (and may be more efficient) for obtaining the Node 
 * and then obtaining Node's HAB:
 * @code
 * bionet_node_get_hab(bionet_stream_get_node(resource));
 * @endcode
 */
BIONET_UTIL_API_DECL
bionet_hab_t * bionet_stream_get_hab(const bionet_stream_t *stream);


/**
 * @brief Get the parent Node of a Stream
 *
 * @param[in] stream Pointer to a Stream
 * 
 * @return Pointer to the Stream's Node
 * @return NULL on failure
 */
BIONET_UTIL_API_DECL
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
BIONET_UTIL_API_DECL
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
BIONET_UTIL_API_DECL
bionet_stream_direction_t bionet_stream_get_direction(const bionet_stream_t * stream);


/**
 * @brief Convert a text string describing a Stream direction into
 *        the appropriate bionet_stream_direction_t.
 *
 * @param[in] direction_string The string to convert.
 *
 * @return The bionet_stream_direction_t.
 */
BIONET_UTIL_API_DECL
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
BIONET_UTIL_API_DECL
const char *bionet_stream_direction_to_string(bionet_stream_direction_t direction);


/**
 * @brief Get the Bionet qualified name of the Stream
 *
 * Stream name is of the format <HAB-Type>.<HAB-ID>.<Node-ID>:<Stream-ID>
 *
 * @param[in] stream The Stream
 *
 * @return The name of the Stream, or NULL on error.
 *
 * @note This function is shorthand (and may be more efficient) for the following code. Internal 
 * library memory is used and cleaned up by the library when the node is free'd.
 * @code
 * char str[BIONET_NAME_COMPONENT_MAX_LEN * 4];
 * sprintf(str, "%s.%s.%s:%s", 
 *         bionet_hab_get_type(bionet_stream_get_hab(stream)), 
 *         bionet_hab_get_id(bionet_stream_get_hab(stream)),
 *         bionet_node_get_id(bionet_stream_get_node(stream)),
 *         bionet_stream_get_id(stream));
 * @endcode
 */
BIONET_UTIL_API_DECL
const char *bionet_stream_get_name(const bionet_stream_t *stream);


/**
 * @brief Get the local-to-the-HAB name of a Stream
 *
 * The local Stream name is of the format <Node-ID>:<Stream-ID>
 *
 * @param[in] stream The Stream
 *
 * @return The local name of the Stream, or NULL on error.
 *
 * @note This function is shorthand (and may be more efficient) for the following code. Internal 
 * library memory is used and cleaned up by the library when the node is free'd.
 * @code
 * char str[BIONET_NAME_COMPONENT_MAX_LEN * 2];
 * sprintf(str, "%s:%s", 
 *         bionet_node_get_id(bionet_stream_get_node(stream)),
 *         bionet_stream_get_id(stream));
 * @endcode
 */
BIONET_UTIL_API_DECL
const char *bionet_stream_get_local_name(const bionet_stream_t *stream);


/**
 * @brief Set the user-data annotation of a Stream
 *
 * @param[in] stream The Stream
 * @param[in] user_data The data to annotate the stream with.
 *
 * @note If the user sets the user data, then the user is 
 *       responsible for freeing the it and setting it to 
 *       NULL before the hab is free'd.
 */
BIONET_UTIL_API_DECL
void bionet_stream_set_user_data(bionet_stream_t *stream, const void *user_data);


/**
 * @brief Get the user-data annotation of a Stream
 *
 * @param[in] stream The Stream
 *
 * @return The user_data pointer, or NULL if none has been set.
 */
BIONET_UTIL_API_DECL
void *bionet_stream_get_user_data(const bionet_stream_t *stream);


#ifdef __cplusplus
}
#endif


#endif // __BIONET_STREAM_H

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
