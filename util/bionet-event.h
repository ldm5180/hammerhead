
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef __BIONET_EVENT_H
#define __BIONET_EVENT_H

/**
 * @file bionet-event.h
 * Functions for dealing with Bionet Events.
 */


#include "bionet-platform.h"

#include "libbionet-util-decl.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Type of event
 *
 */
typedef enum {
    BIONET_EVENT_PUBLISHED = 0,
    BIONET_EVENT_LOST
} bionet_event_type_t;


/**
 * @brief Obtain a pointer to a new bionet_event_t
 *
 * @param[in] timestamp Timestamp of this new event or NULL to use the current time
 * @param[in] bdm_id BDM ID That recorded this event. Memory will be copied
 * @param[in] event Type of event.
 *
 * @return Valid pointer on success
 * @return NULL on failure
 */
BIONET_UTIL_API_DECL
bionet_event_t * bionet_event_new(const struct timeval *timestamp, const char * bdm_id, bionet_event_type_t type);

/**
 * @brief Free a bionet_event_t
 *
 * @param[in] event Pointer to free
 *
 */
BIONET_UTIL_API_DECL
void bionet_event_free(bionet_event_t *event);


/**
 * @brief Get the event's timestamp
 *
 * @param[in] event The event
 *
 * @return Pointer to a struct timeval in the event
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
const struct timeval * bionet_event_get_timestamp(const bionet_event_t * event);

/**
 * @brief Get the event's timestamp as a string
 *
 * @param[in] event The event
 *
 * @return Pointer to new buffer, which must be deallocated with free()
 */
BIONET_UTIL_API_DECL
char * bionet_event_get_timestamp_as_str(
        const bionet_event_t * event);


/**
 * @brief Get the id of the bdm that recorded this event
 *
 * @param[in] event The event
 *
 * @return Pointer to a const char *
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
const char * bionet_event_get_bdm_id(const bionet_event_t * event);


/**
 * @brief Get the event type
 *
 * @param[in] event The event
 *
 * @return Pointer to a bionet_bdm_t
 * @return NULL Error
 */
BIONET_UTIL_API_DECL
bionet_event_type_t bionet_event_get_type(const bionet_event_t * event);


/**
 * @brief Get the event entry sequence number
 *
 * Sequence numbers are valid only for a singel reporting bdm
 *
 * @param[in] event The event
 *
 * @return Sequence number set for event, or -1 if not set
 */
BIONET_UTIL_API_DECL
int64_t bionet_event_get_seq(const bionet_event_t * event);


/**
 * @brief Set the event entry sequence number
 *
 * Sequence numbers are valid only for a singel reporting bdm
 *
 * @param[in] event The event
 * @param[in] seq The sequence number
 */
BIONET_UTIL_API_DECL
void bionet_event_set_seq(bionet_event_t * event, int64_t seq);


/**
 * @brief Set the user-data annotation of a Event
 *
 * @param[in] event The Event
 * @param[in] user_data The data to annotate the Event with.
 *
 * @note If the user sets the user data, then the user is 
 *       responsible for freeing the it and setting it to 
 *       NULL before the event is free'd.
 */
BIONET_UTIL_API_DECL
void bionet_event_set_user_data(bionet_event_t *event, const void *user_data);


/**
 * @brief Get the user-data annotation of a Event
 *
 * @param[in] event The Event
 *
 * @return The user_data pointer, or NULL if none has been set.
 */
BIONET_UTIL_API_DECL
void *bionet_event_get_user_data(const bionet_event_t *event);


/**
 * @brief Increment the reference count
 *
 * This function is used by wrappers of this interface. It is not
 * needed for writing C, but is for SWIG-generated Python so that
 * garbage collection works properly.
 *
 * @param[in] event Event to increment the reference count for
 */
BIONET_UTIL_API_DECL
void bionet_event_increment_ref_count(bionet_event_t * event);


/**
 * @brief Get the reference count
 *
 * This function is used by wrappers of this interface. It is not
 * needed for writing C, but is for SWIG-generated Python so that
 * garbage collection works properly.
 *
 * @param[in] event Event to get the reference count for
 *
 * @return Number of references currently held.
 */
BIONET_UTIL_API_DECL
unsigned int bionet_event_get_ref_count(bionet_event_t * event);


#ifdef __cplusplus
}
#endif

#endif /* __BIONET_EVENT_H */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
