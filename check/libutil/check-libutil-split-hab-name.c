
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <check.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "check-common.h"
#include "bionet-util.h"
#include "check-libutil-hab-tests.h"


START_TEST (test_libutil_split_hab_name_0) {
    char *hab_type;
    char *hab_id;
    int r;

    r = bionet_split_hab_name(NULL, &hab_type, &hab_id);
    fail_if(r != -1, "failed to reject NULL HAB Name\n");

    r = bionet_split_hab_name("hello.world", NULL, &hab_id);
    fail_if(r != -1, "failed to reject NULL HAB Type\n");

    r = bionet_split_hab_name("hello.world", &hab_type, NULL);
    fail_if(r != -1, "failed to reject NULL HAB ID\n");
} END_TEST


void libutil_split_hab_name_suite(Suite *s) {
    TCase *tc = tcase_create("bionet_split_hab_name()");
    suite_add_tcase(s, tc);

    tcase_add_test(tc, test_libutil_split_hab_name_0);

    return;
}

