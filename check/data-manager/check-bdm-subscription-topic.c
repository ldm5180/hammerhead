
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <check.h>

#include "check-common.h"
#include "bionet-util.h"
#include "check-bdm.h"
#include "bionet-util.h"
#include "data-manager/server/bionet-data-manager.h"

struct _test_tuple {
    const char * topic;
    const char * subscription;
    const int match;
};

static struct _test_tuple sub_match_data[] = {

    { 
        "D bdm/ht.hi.n:res?tsmin=1&tsmax=1",
        "D *.*.*:*?dpstart=1&dpend=2",
        1
    },

    { 
        "D bdm/ht.hi.n:res?tsmin=1&tsmax=1",
        "D *.*.*:*?dpstart=2&dpend=2",
        0
    },

    { // Overlapping time range
        "D bdm/ht.hi.n:res?tsmin=1&tsmax=1.500000",
        "D *.*.*:*?dpstart=1.500000&dpend=2.000001",
        1
    },
    {  // Datapoints time before subscription
        "D bdm/ht.hi.n:res?tsmin=1&tsmax=1.500000",
        "D *.*.*:*?dpstart=1.500001&dpend=2.000001",
        0
    },

    {  // Overlapping range
        "D bdm/ht.hi.n:res?tsmin=2&tsmax=2.500000",
        "D *.*.*:*?dpstart=1.499999&dpend=2.000000",
        1
    },
    { // Datapoints time after subscription
        "D bdm/ht.hi.n:res?tsmin=2&tsmax=2.500000",
        "D *.*.*:*?dpstart=1.500001&dpend=1.999999",
        0
    },

    { // bdm matches; No time range filter
        "D bdm/ht.hi.n:res?tsmin=1&tsmax=1.500000",
        "D bdm/*.*.*:*",
        1
    },
    { // bdm unspecified; No time range filter
        "D bdm/ht.hi.n:res?tsmin=1&tsmax=1.500000",
        "D */*.*.*:*",
        1
    },
    { // bdm unspecified; No time range filter
        "D bdm/ht.hi.n:res?tsmin=1&tsmax=1.500000",
        "D *.*.*:*",
        1
    },
    { // bdm doesn't match; No time range filter
        "D bdm/ht.hi.n:res?tsmin=1&tsmax=1.500000",
        "D nomatch/*.*.*:*",
        0
    },
};

START_TEST (test_bdm_subscription_matching) {

    struct _test_tuple *data = &sub_match_data[_i];

    int r = libbdm_cal_topic_matches(data->topic, data->subscription);

    fail_if((data->match && r<0), 
            "Subscription '%s' should match topic '%s', but didn't",
            data->subscription, data->topic);

    fail_if((!data->match && r==0), 
            "Subscription '%s' should NOT match topic '%s', but did",
            data->subscription, data->topic);

} END_TEST /* test_bdm_subscription_matching */


void bdm_subscription_topic_suite(Suite *s)
{
    TCase *tc = tcase_create("data_manager_config_suite()");
    suite_add_tcase(s, tc);

    tcase_add_loop_test(tc, test_bdm_subscription_matching, 
            0, SIZEOF(sub_match_data));


    return;
} /* libutil_hab_tests_suite() */


// vim: ts=8 sw=4 sta expandtab
// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// indent-tabs-mode: nil
// End:
