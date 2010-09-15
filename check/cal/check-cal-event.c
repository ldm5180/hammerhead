
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <check.h>

#include "cal-event.h"

#include "check-common.h"
#include "check-cal.h"




START_TEST (test_cal_event_free_NULL) {
    cal_event_free(NULL);
} END_TEST




START_TEST (test_cal_event_free_empty) {
    cal_event_t *e;

    e = cal_event_new(CAL_EVENT_JOIN);
    fail_if(e == NULL, "failed to create a Join event");
    cal_event_free(e);
} END_TEST




void cal_event_test_suite(Suite *s)
{
    TCase *tc = tcase_create("cal event test suite");
    suite_add_tcase(s, tc);

    tcase_add_test(tc, test_cal_event_free_NULL);
    tcase_add_test(tc, test_cal_event_free_empty);

    return;
} 


// vim: ts=8 sw=4 sta expandtab
// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// indent-tabs-mode: nil
// End:
