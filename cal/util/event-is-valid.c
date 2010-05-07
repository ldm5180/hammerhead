
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include "cal-util.h"


int cal_event_is_valid(const cal_event_t *event) {
    if (event == NULL) return 0;

    switch (event->type) {
        case CAL_EVENT_NONE: {
            return 0;
        }

        case CAL_EVENT_JOIN: {
            if (!cal_peer_name_is_valid(event->peer_name)) return 0;
            if (event->topic != NULL) return 0;
            if (event->msg.buffer != NULL) return 0;
            if (event->msg.size != 0) return 0;
            return 1;
        }

        case CAL_EVENT_LEAVE: {
            if (!cal_peer_name_is_valid(event->peer_name)) return 0;
            if (event->topic != NULL) return 0;
            if (event->msg.buffer != NULL) return 0;
            if (event->msg.size != 0) return 0;
            return 1;
        }

        case CAL_EVENT_CONNECT: {
            if (!cal_peer_name_is_valid(event->peer_name)) return 0;
            if (event->topic != NULL) return 0;
            if (event->msg.buffer != NULL) return 0;
            if (event->msg.size != 0) return 0;
            return 1;
        }

        case CAL_EVENT_DISCONNECT: {
            if (!cal_peer_name_is_valid(event->peer_name)) return 0;
            if (event->topic != NULL) return 0;
            if (event->msg.buffer != NULL) return 0;
            if (event->msg.size != 0) return 0;
            return 1;
        }

        case CAL_EVENT_MESSAGE: {
            if (!cal_peer_name_is_valid(event->peer_name)) return 0;
            if (event->topic != NULL) return 0;
            if (event->msg.buffer == NULL) return 0;
            if (event->msg.size == 0) return 0;
            return 1;
        }

        case CAL_EVENT_SUBSCRIBE: {
            if (!cal_peer_name_is_valid(event->peer_name)) return 0;
            if (!cal_topic_is_valid(event->topic)) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "cal_event_is_valid(): subscribe event with NULL topic");
                return 0;
            }
            if (event->msg.buffer != NULL) return 0;
            if (event->msg.size != 0) return 0;
            return 1;
        }

        case CAL_EVENT_UNSUBSCRIBE: {
            if (!cal_peer_name_is_valid(event->peer_name)) return 0;
            if (!cal_topic_is_valid(event->topic)) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "cal_event_is_valid(): unsubscribe event with NULL topic");
                return 0;
            }
            if (event->msg.buffer != NULL) return 0;
            if (event->msg.size != 0) return 0;
            return 1;
        }

        case CAL_EVENT_PUBLISH: {
            if (event->peer_name == NULL) {
                if (!cal_topic_is_valid(event->topic)) {
                    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "cal_event_is_valid(): publish event with NULL peer and invalid topic");
                    return 0;
                }
            } else {
                if (!cal_peer_name_is_valid(event->peer_name)) return 0;
                if ((event->topic != NULL) && !cal_topic_is_valid(event->topic)) {
                    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "cal_event_is_valid(): publish event with peer has invalid topic");
                    return 0;
                }
            }
            if (event->msg.buffer == NULL) return 0;
            if (event->msg.size == 0) return 0;
            return 1;
        }

        case CAL_EVENT_INIT: {
            if (event->peer_name != NULL) return 0;
            if (event->topic != NULL) return 0;
            if (event->msg.buffer != NULL) return 0;
            if (event->msg.size != 0) return 0;
            return 1;
        }

        case CAL_EVENT_SHUTDOWN: {
            if (event->peer_name != NULL) return 0;
            if (event->topic != NULL) return 0;
            if (event->msg.buffer != NULL) return 0;
            if (event->msg.size != 0) return 0;
            return 1;
        }

        default: {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "cal_event_is_valid(): unknown event type %d passed in", event->type);
            return 0;
        }
    }

    // not reached
    return 1;
}

