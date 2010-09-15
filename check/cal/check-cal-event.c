
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




START_TEST (test_cal_event_is_valid_None) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_NONE);
    fail_if(e == NULL, "failed to create a None event");

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a None event was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Subscribe_without_peer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_SUBSCRIBE);
    fail_if(e == NULL, "failed to create a Subscribe event");

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Subscribe event without a peer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Subscribe_with_peer_but_no_topic) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_SUBSCRIBE);
    fail_if(e == NULL, "failed to create a Subscribe event");

    e->peer_name = "dummy-peer";

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Subscribe event with a peer but without a topic was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Unsubscribe_without_peer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_UNSUBSCRIBE);
    fail_if(e == NULL, "failed to create an Unsubscribe event");

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, an Unsubscribe event without a peer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Unsubscribe_with_peer_but_no_topic) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_UNSUBSCRIBE);
    fail_if(e == NULL, "failed to create an Unsubscribe event");

    e->peer_name = "dummy-peer";

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, an Unsubscribe event with a peer but no topic was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Publish_with_no_peer_and_invalid_topic) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_PUBLISH);
    fail_if(e == NULL, "failed to create a Publish event");

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Publish event with NULL peer and with an invalid topic was accepted as valid");
} END_TEST




void cal_event_test_suite(Suite *s)
{
    TCase *tc = tcase_create("cal event test suite");
    suite_add_tcase(s, tc);

    tcase_add_test(tc, test_cal_event_free_NULL);
    tcase_add_test(tc, test_cal_event_free_empty);

    tcase_add_test(tc, test_cal_event_is_valid_None);

    tcase_add_test(tc, test_cal_event_is_valid_Subscribe_without_peer);
    tcase_add_test(tc, test_cal_event_is_valid_Subscribe_with_peer_but_no_topic);

    tcase_add_test(tc, test_cal_event_is_valid_Unsubscribe_without_peer);
    tcase_add_test(tc, test_cal_event_is_valid_Unsubscribe_with_peer_but_no_topic);

    tcase_add_test(tc, test_cal_event_is_valid_Publish_with_no_peer_and_invalid_topic);

    return;
} 


// vim: ts=8 sw=4 sta expandtab
// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// indent-tabs-mode: nil
// End:
