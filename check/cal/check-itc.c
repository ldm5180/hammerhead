
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
#include "bip-itc.h"

#ifdef __WIN32
#include <winsock2.h>
#include <windows.h>
#endif


START_TEST (test_itc_pipe) {

    int r;
    bip_msg_queue_t q;

#ifdef __WIN32
    int ret;
    WSADATA wsaData;
    ret = WSAStartup(0x0202, &wsaData);
    if ( ret ) {
        fail("WSAStartup() failed with error %d\n", ret); 
    }

#endif

    cal_event_t a_real_event = {
        .type = CAL_EVENT_JOIN,
        .peer_name = "some random peer",
        .topic = NULL,
        .msg.buffer = NULL,
        .msg.size = 0,
    };

    cal_event_t * event = &a_real_event;
    cal_event_t *event_recvd = NULL;

    r = bip_msg_queue_init(&q);
    fail_unless(r == 0, "bip_msg_queue_init() returned %d", r);

    r = bip_msg_queue_push(&q, BIP_MSG_QUEUE_FROM_USER, event);
    fail_unless(r == 0, "bip_msg_queue_push() returned %d", r);

    r = bip_msg_queue_pop(&q, BIP_MSG_QUEUE_FROM_USER, &event_recvd);
    fail_unless(r == 0, "bip_msg_queue_pop() returned %d", r);
    fail_unless(event_recvd == event, "bip_msg_queue_pop() returned wrong event");

    r = bip_msg_queue_close(&q, BIP_MSG_QUEUE_FROM_USER);
    fail_unless(r == 0, "bip_msg_queue_close() returned %d", r);

    r = bip_msg_queue_pop(&q, BIP_MSG_QUEUE_FROM_USER, &event_recvd);
    fail_unless(r == 1, "bip_msg_queue_pop() returned %d when closed", r);

    r = bip_msg_queue_close(&q, BIP_MSG_QUEUE_TO_USER);
    fail_unless(r == 0, "bip_msg_queue_close() returned %d", r);



} END_TEST /* test_itc_pipe */


void bionet_cal_test_suite(Suite *s)
{
    TCase *tc = tcase_create("bionet_cal_test_suite()");
    suite_add_tcase(s, tc);

    tcase_add_test(tc, test_itc_pipe);

    return;
} 


// vim: ts=8 sw=4 sta expandtab
// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// indent-tabs-mode: nil
// End:
