
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include "libhab-internal.h"


int libhab_cal_topic_matches(const char *topic, const char *subscription) {
    if (bionet_name_component_matches(topic, subscription)) return 0;
    return -1;
}

