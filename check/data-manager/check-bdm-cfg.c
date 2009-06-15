
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
#include "data-manager/server/bionet-data-manager.h"

static const char * good_config_files[] = {
    "config-data/good-1.cfg"
};

static const sync_sender_config_t good_configs[] = {
    { /* good-1.cfg */
        BDM_SYNC_METHOD_TCP,
        {1, 0},
        {0, 0},
        {0, 0},
        "*.*.*:*",
        5,
        "localhost"
    }
};


static void _verify_cfg_same(
    sync_sender_config_t * cfg1,
    sync_sender_config_t * cfg2)
{

    fail_unless(cfg1->method == cfg2->method,
        "Incorrect method");
    fail_unless(cfg1->start_time.tv_sec == cfg2->start_time.tv_sec
        && cfg1->start_time.tv_usec == cfg2->start_time.tv_usec,
        "Incorrect start_time %d vs %d", cfg1->start_time.tv_sec, cfg2->start_time.tv_sec);
    fail_unless(cfg1->end_time.tv_sec == cfg2->end_time.tv_sec
        && cfg1->end_time.tv_usec == cfg2->end_time.tv_usec,
        "Incorrect end_time");
    fail_unless(
        !strcmp(cfg1->resource_name_pattern, cfg2->resource_name_pattern),
        "Incorrect resource_name_pattern");
    fail_unless(cfg1->frequency == cfg2->frequency,
        "Incorrect frequency");
    fail_unless(
        !strcmp(cfg1->sync_recipient, cfg2->sync_recipient),
        "Incorrect sync_recipient");

}

START_TEST (test_bdm_cfg_read_good) {
    sync_sender_config_t * cfg = 
        read_config_file(good_config_files[_i]);

    fail_if(NULL == cfg, "Config read failed");

    _verify_cfg_same(cfg, &good_configs[_i]);

} END_TEST /* test_bdm_cfg_read_good */


void data_manager_config_suite(Suite *s)
{
    TCase *tc = tcase_create("data_manager_config_suite()");
    suite_add_tcase(s, tc);


    tcase_add_loop_test(tc, test_bdm_cfg_read_good, 0, SIZEOF(good_config_files));


    return;
} /* libutil_hab_tests_suite() */


// vim: ts=8 sw=4 sta expandtab
// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// indent-tabs-mode: nil
// End:
