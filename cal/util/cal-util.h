
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef __CAL_UTIL_H
#define __CAL_UTIL_H


#include "cal-event.h"


#define  CAL_LOG_DOMAIN  "CAL"




// CAL peer names may not be longer than this many bytes (including the
// terminating NULL).
// Note: This fits a 200 byte Bionet HAB Name comfortably
#define CAL_PEER_NAME_MAX_LENGTH  (512)




// CAL subscription topics may not be longer than this many bytes
// (including the terminating NULL).
// Note: This fits a 400+ byte Bionet Datapoint Name comfortably
#define CAL_TOPIC_MAX_LENGTH  (512)




/**
 * @brief Validates a CAL event.
 *
 * @param[in] event The event to check.
 *
 * @retval 1 event is valid and may be used.
 *
 * @retval 0 event is invalid and should be discarded.
 **/
int cal_event_is_valid(const cal_event_t *event);


/**
 * @brief Validates a CAL peer name.
 *
 * @param[in] peer_name The peer name to check.
 *
 * @retval 1 peer_name is valid (all printable, not more than
 *     CAL_PEER_NAME_MAX_LENGTH bytes long, and NULL terminated).
 *
 * @retval 0 peer_name is invalid
 **/
int cal_peer_name_is_valid(const char *peer_name);


/**
 * @brief Validates a CAL subscription topic.
 *
 * @param[in] topic The topic to check.
 *
 * @retval 1 topic is valid (all printable, not more than
 *     CAL_TOPIC_MAX_LENGTH bytes long, and NULL terminated).
 * @retval 0 topic is invalid
 **/
int cal_topic_is_valid(const char *topic);


#endif //  __CAL_UTIL_H

