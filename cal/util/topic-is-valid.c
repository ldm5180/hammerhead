
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include "cal-util.h"


// coverity[ -tainted_data_sink : arg-0 ]
int cal_topic_is_valid(const char *topic) {
    int i;

    if (topic == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "cal_topic_is_valid(): NULL topic passed in");
        return 0;
    }

    for (i = 0; i < CAL_TOPIC_MAX_LENGTH; i ++) {
        if (topic[i] == (char)0) break;
        if (isprint(topic[i])) continue;
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "cal_topic_is_valid(): byte at offset %d of topic \"%s\" is invalid", i, topic);
        return 0;
    }
    if (i == 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "cal_topic_is_valid(): empty topic passed in");
        return 0;
    }
    if (i == CAL_TOPIC_MAX_LENGTH) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "cal_topic_is_valid(): passed-in topic is too long (max %d)", CAL_TOPIC_MAX_LENGTH);
        return 0;
    }

    // it's ok
    return 1;
}

