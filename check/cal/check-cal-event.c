
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




START_TEST (test_cal_event_is_valid_NULL) {
    cal_event_is_valid(NULL);
} END_TEST




START_TEST (test_cal_event_is_valid_None) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_NONE);
    fail_if(e == NULL, "failed to create a None event");

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a None event was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Join_without_peer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_JOIN);
    fail_if(e == NULL, "failed to create a Join event");

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Join event without a peer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Join_with_peer_and_topic) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_JOIN);
    fail_if(e == NULL, "failed to create a Join event");

    e->peer_name = "dummy-peer";
    e->topic = "dummy-topic";

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Join event with a topic was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Join_with_peer_and_msg_buffer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_JOIN);
    fail_if(e == NULL, "failed to create a Join event");

    e->peer_name = "dummy-peer";
    e->msg.buffer = strdup("dummy-topic");
    e->msg.size = 0;

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Join event with a msg.buffer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Join_with_peer_and_nonzero_msg_size) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_JOIN);
    fail_if(e == NULL, "failed to create a Join event");

    e->peer_name = "dummy-peer";
    e->msg.size = 999;

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Join event with a non-zero msg.size was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Leave_without_peer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_LEAVE);
    fail_if(e == NULL, "failed to create a Leave event");

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Leave event without a peer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Leave_with_peer_and_topic) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_LEAVE);
    fail_if(e == NULL, "failed to create a Leave event");

    e->peer_name = "dummy-peer";
    e->topic = "dummy-topic";

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Leave event with a topic was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Leave_with_peer_and_msg_buffer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_LEAVE);
    fail_if(e == NULL, "failed to create a Leave event");

    e->peer_name = "dummy-peer";
    e->msg.buffer = strdup("dummy-topic");
    e->msg.size = 0;

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Leave event with a msg.buffer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Leave_with_peer_and_nonzero_msg_size) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_LEAVE);
    fail_if(e == NULL, "failed to create a Leave event");

    e->peer_name = "dummy-peer";
    e->msg.size = 999;

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Leave event with a non-zero msg.size was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Connect_without_peer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_CONNECT);
    fail_if(e == NULL, "failed to create a Connect event");

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Connect event without a peer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Connect_with_peer_and_topic) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_CONNECT);
    fail_if(e == NULL, "failed to create a Connect event");

    e->peer_name = "dummy-peer";
    e->topic = "dummy-topic";

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Connect event with a topic was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Connect_with_peer_and_msg_buffer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_CONNECT);
    fail_if(e == NULL, "failed to create a Connect event");

    e->peer_name = "dummy-peer";
    e->msg.buffer = strdup("dummy-topic");
    e->msg.size = 0;

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Connect event with a msg.buffer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Connect_with_peer_and_nonzero_msg_size) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_CONNECT);
    fail_if(e == NULL, "failed to create a Connect event");

    e->peer_name = "dummy-peer";
    e->msg.size = 999;

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Connect event with a non-zero msg.size was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Disconnect_without_peer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_DISCONNECT);
    fail_if(e == NULL, "failed to create a Disconnect event");

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Disconnect event without a peer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Disconnect_with_peer_and_topic) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_DISCONNECT);
    fail_if(e == NULL, "failed to create a Disconnect event");

    e->peer_name = "dummy-peer";
    e->topic = "dummy-topic";

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Disconnect event with a topic was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Disconnect_with_peer_and_msg_buffer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_DISCONNECT);
    fail_if(e == NULL, "failed to create a Disconnect event");

    e->peer_name = "dummy-peer";
    e->msg.buffer = strdup("dummy-topic");
    e->msg.size = 0;

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Disconnect event with a msg.buffer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Disconnect_with_peer_and_nonzero_msg_size) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_DISCONNECT);
    fail_if(e == NULL, "failed to create a Disconnect event");

    e->peer_name = "dummy-peer";
    e->msg.size = 999;

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Disconnect event with a non-zero msg.size was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Message_without_peer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_MESSAGE);
    fail_if(e == NULL, "failed to create a Message event");

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Message event without a peer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Message_with_peer_and_topic) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_MESSAGE);
    fail_if(e == NULL, "failed to create a Message event");

    e->peer_name = "dummy-peer";
    e->topic = "dummy-topic";

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Message event with a topic was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Message_with_peer_and_NULL_msg_buffer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_MESSAGE);
    fail_if(e == NULL, "failed to create a Message event");

    e->peer_name = "dummy-peer";

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Message event with a NULL msg.buffer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Message_with_peer_and_zero_msg_size) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_MESSAGE);
    fail_if(e == NULL, "failed to create a Message event");

    e->peer_name = "dummy-peer";
    e->msg.buffer = strdup("dummy message");
    e->msg.size = 0;

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Message event with a zero msg.size was accepted as valid");
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




START_TEST (test_cal_event_is_valid_Subscribe_with_peer_and_topic_and_msg_buffer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_SUBSCRIBE);
    fail_if(e == NULL, "failed to create a Subscribe event");

    e->peer_name = "dummy-peer";
    e->topic = "dummy-topic";

    e->msg.buffer = strdup("dummy-msg");

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Subscribe event with a peer and topic and a non-Null msg.buffer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Subscribe_with_peer_and_topic_and_nonzero_msg_size) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_SUBSCRIBE);
    fail_if(e == NULL, "failed to create a Subscribe event");

    e->peer_name = "dummy-peer";
    e->topic = "dummy-topic";

    e->msg.size = 12345;

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Subscribe event with a peer and topic and a non-zero msg.size was accepted as valid");
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




START_TEST (test_cal_event_is_valid_Unsubscribe_with_peer_and_topic_and_msg_buffer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_UNSUBSCRIBE);
    fail_if(e == NULL, "failed to create an Unsubscribe event");

    e->peer_name = "dummy-peer";
    e->topic = "dummy-topic";

    e->msg.buffer = strdup("dummy-message");

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, an Unsubscribe event with a peer and topic and msg.buffer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Unsubscribe_with_peer_and_topic_and_nonzero_msg_size) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_UNSUBSCRIBE);
    fail_if(e == NULL, "failed to create an Unsubscribe event");

    e->peer_name = "dummy-peer";
    e->topic = "dummy-topic";

    e->msg.size = 4544;

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, an Unsubscribe event with a peer and topic and nonzero msg.size was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Publish_with_no_peer_and_no_topic) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_PUBLISH);
    fail_if(e == NULL, "failed to create a Publish event");

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Publish event with NULL peer and with an invalid topic was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Publish_with_peer_but_invalid_topic) {
    cal_event_t *e;
    int r;
    char invalid_topic[] = { 'H', 'i', 0xff, 0x01 };

    e = cal_event_new(CAL_EVENT_PUBLISH);
    fail_if(e == NULL, "failed to create a Publish event");

    e->peer_name = "dummy-peer";
    e->topic = invalid_topic;

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Publish event with a peer but invalid topic was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Publish_with_invalid_non_NULL_peer) {
    cal_event_t *e;
    int r;
    char invalid_peer[] = { 'H', 'i', 0x0a };

    e = cal_event_new(CAL_EVENT_PUBLISH);
    fail_if(e == NULL, "failed to create a Publish event");

    e->peer_name = invalid_peer;

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Publish event with a non-NULL but invalid peer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Publish_with_NULL_peer_and_valid_topic_but_NULL_msg_buffer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_PUBLISH);
    fail_if(e == NULL, "failed to create a Publish event");

    e->topic = strdup("dummy topic");

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Publish event with a NULL peer and valid topic but NULL msg.buffer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Publish_with_NULL_peer_and_valid_topic_and_non_NULL_msg_buffer_but_zero_msg_size) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_PUBLISH);
    fail_if(e == NULL, "failed to create a Publish event");

    e->topic = strdup("dummy topic");

    e->msg.buffer = "dummy message";

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, a Publish event with a NULL peer and valid topic but non-zero msg.size was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Init_with_peer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_INIT);
    fail_if(e == NULL, "failed to create an Init event");

    e->peer_name = "dummy-peer";

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, an Init event with a peer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Init_with_topic) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_INIT);
    fail_if(e == NULL, "failed to create an Init event");

    e->topic = "dummy-topic";

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, an Init event with a topic was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Init_with_msg_buffer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_INIT);
    fail_if(e == NULL, "failed to create an Init event");

    e->msg.buffer = "dummy-topic";

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, an Init event with a non-NULL msg.buffer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Init_with_msg_size) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_INIT);
    fail_if(e == NULL, "failed to create an Init event");

    e->msg.size = 1;

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, an Init event with a non-zero msg.size was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Shutdown_with_peer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_SHUTDOWN);
    fail_if(e == NULL, "failed to create an Shutdown event");

    e->peer_name = "dummy-peer";

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, an Shutdown event with a peer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Shutdown_with_topic) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_SHUTDOWN);
    fail_if(e == NULL, "failed to create an Shutdown event");

    e->topic = "dummy-topic";

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, an Shutdown event with a topic was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Shutdown_with_msg_buffer) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_SHUTDOWN);
    fail_if(e == NULL, "failed to create an Shutdown event");

    e->msg.buffer = "dummy-topic";

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, an Shutdown event with a non-NULL msg.buffer was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Shutdown_with_msg_size) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_SHUTDOWN);
    fail_if(e == NULL, "failed to create an Shutdown event");

    e->msg.size = 1;

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, an Shutdown event with a non-zero msg.size was accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_Shutdown_valid) {
    cal_event_t *e;
    int r;

    e = cal_event_new(CAL_EVENT_SHUTDOWN);
    fail_if(e == NULL, "failed to create an Shutdown event");

    r = cal_event_is_valid(e);
    fail_if(r == 0, "oh no, a valid Shutdown event was not accepted as valid");
} END_TEST




START_TEST (test_cal_event_is_valid_unknown_type) {
    cal_event_t *e;
    int r;

    e = cal_event_new(9999);
    fail_if(e == NULL, "failed to create an event with some invalid event type");

    r = cal_event_is_valid(e);
    fail_if(r != 0, "oh no, an event with some crazy event type was accepted as valid");
} END_TEST




START_TEST (test_cal_peer_name_is_valid_zero_length) {
    int r;

    r = cal_peer_name_is_valid("");
    fail_if(r != 0, "oh no, a zero-length peer name was accepted as valid");
} END_TEST




void cal_event_test_suite(Suite *s)
{
    TCase *tc = tcase_create("cal event test suite");
    suite_add_tcase(s, tc);

    tcase_add_test(tc, test_cal_event_free_NULL);
    tcase_add_test(tc, test_cal_event_free_empty);

    tcase_add_test(tc, test_cal_event_is_valid_NULL);

    tcase_add_test(tc, test_cal_event_is_valid_None);

    tcase_add_test(tc, test_cal_event_is_valid_Join_without_peer);
    tcase_add_test(tc, test_cal_event_is_valid_Join_with_peer_and_topic);
    tcase_add_test(tc, test_cal_event_is_valid_Join_with_peer_and_msg_buffer);
    tcase_add_test(tc, test_cal_event_is_valid_Join_with_peer_and_nonzero_msg_size);

    tcase_add_test(tc, test_cal_event_is_valid_Leave_without_peer);
    tcase_add_test(tc, test_cal_event_is_valid_Leave_with_peer_and_topic);
    tcase_add_test(tc, test_cal_event_is_valid_Leave_with_peer_and_msg_buffer);
    tcase_add_test(tc, test_cal_event_is_valid_Leave_with_peer_and_nonzero_msg_size);

    tcase_add_test(tc, test_cal_event_is_valid_Connect_without_peer);
    tcase_add_test(tc, test_cal_event_is_valid_Connect_with_peer_and_topic);
    tcase_add_test(tc, test_cal_event_is_valid_Connect_with_peer_and_msg_buffer);
    tcase_add_test(tc, test_cal_event_is_valid_Connect_with_peer_and_nonzero_msg_size);

    tcase_add_test(tc, test_cal_event_is_valid_Disconnect_without_peer);
    tcase_add_test(tc, test_cal_event_is_valid_Disconnect_with_peer_and_topic);
    tcase_add_test(tc, test_cal_event_is_valid_Disconnect_with_peer_and_msg_buffer);
    tcase_add_test(tc, test_cal_event_is_valid_Disconnect_with_peer_and_nonzero_msg_size);

    tcase_add_test(tc, test_cal_event_is_valid_Message_without_peer);
    tcase_add_test(tc, test_cal_event_is_valid_Message_with_peer_and_topic);
    tcase_add_test(tc, test_cal_event_is_valid_Message_with_peer_and_NULL_msg_buffer);
    tcase_add_test(tc, test_cal_event_is_valid_Message_with_peer_and_zero_msg_size);

    tcase_add_test(tc, test_cal_event_is_valid_Subscribe_without_peer);
    tcase_add_test(tc, test_cal_event_is_valid_Subscribe_with_peer_but_no_topic);
    tcase_add_test(tc, test_cal_event_is_valid_Subscribe_with_peer_and_topic_and_msg_buffer);
    tcase_add_test(tc, test_cal_event_is_valid_Subscribe_with_peer_and_topic_and_nonzero_msg_size);

    tcase_add_test(tc, test_cal_event_is_valid_Unsubscribe_without_peer);
    tcase_add_test(tc, test_cal_event_is_valid_Unsubscribe_with_peer_but_no_topic);
    tcase_add_test(tc, test_cal_event_is_valid_Unsubscribe_with_peer_and_topic_and_msg_buffer);
    tcase_add_test(tc, test_cal_event_is_valid_Unsubscribe_with_peer_and_topic_and_nonzero_msg_size);

    tcase_add_test(tc, test_cal_event_is_valid_Publish_with_no_peer_and_no_topic);
    tcase_add_test(tc, test_cal_event_is_valid_Publish_with_invalid_non_NULL_peer);
    tcase_add_test(tc, test_cal_event_is_valid_Publish_with_peer_but_invalid_topic);
    tcase_add_test(tc, test_cal_event_is_valid_Publish_with_NULL_peer_and_valid_topic_but_NULL_msg_buffer);
    tcase_add_test(tc, test_cal_event_is_valid_Publish_with_NULL_peer_and_valid_topic_and_non_NULL_msg_buffer_but_zero_msg_size);

    tcase_add_test(tc, test_cal_event_is_valid_Init_with_peer);
    tcase_add_test(tc, test_cal_event_is_valid_Init_with_topic);
    tcase_add_test(tc, test_cal_event_is_valid_Init_with_msg_buffer);
    tcase_add_test(tc, test_cal_event_is_valid_Init_with_msg_size);

    tcase_add_test(tc, test_cal_event_is_valid_Shutdown_with_peer);
    tcase_add_test(tc, test_cal_event_is_valid_Shutdown_with_topic);
    tcase_add_test(tc, test_cal_event_is_valid_Shutdown_with_msg_buffer);
    tcase_add_test(tc, test_cal_event_is_valid_Shutdown_with_msg_size);
    tcase_add_test(tc, test_cal_event_is_valid_Shutdown_valid);

    tcase_add_test(tc, test_cal_event_is_valid_unknown_type);

    tcase_add_test(tc, test_cal_peer_name_is_valid_zero_length);

    return;
}


// vim: ts=8 sw=4 sta expandtab
// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// indent-tabs-mode: nil
// End:
