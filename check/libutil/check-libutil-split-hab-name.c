
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


// misc NULLs passed in
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




// misc valid inputs
// BIONET_NAME_COMPONENT_MAX_LEN is the max length of a name component, including the terminating NULL
START_TEST (test_libutil_split_hab_name_1) {
    char *hab_type;
    char *hab_id;
    char *name;
    int r;

    int i;
    char name_str[BIONET_NAME_COMPONENT_MAX_LEN * 2];

    name = "HAB-Type.HAB-ID";
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != 0, "failed to accept valid HAB Name %s\n", name);

    name = "a.b";
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != 0, "failed to accept valid HAB Name %s\n", name);

    name = "*.*";
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != 0, "failed to accept valid HAB Name %s\n", name);

    name = "type.*";
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != 0, "failed to accept valid HAB Name %s\n", name);

    name = "*.id";
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != 0, "failed to accept valid HAB Name %s\n", name);

    name = "1.2";
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != 0, "failed to accept valid HAB Name %s\n", name);

    // valid chars are [-a-zA-Z0-9]
    name = "--.--";
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != 0, "failed to accept valid HAB Name %s\n", name);

    for (i = 0; i < (BIONET_NAME_COMPONENT_MAX_LEN-1); i ++) {
        name_str[i] = 'A' + (i % 26);
        name_str[i+BIONET_NAME_COMPONENT_MAX_LEN] = 'a' + (i % 26);
    }
    name_str[BIONET_NAME_COMPONENT_MAX_LEN-1] = '.';
    name_str[(2*BIONET_NAME_COMPONENT_MAX_LEN)-1] = 0;
    name = name_str;
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != 0, "failed to accept valid HAB Name %s\n", name);

    for (i = 0; i < (BIONET_NAME_COMPONENT_MAX_LEN-1); i ++) {
        name_str[i] = '0' + (i % 10);
        name_str[i+BIONET_NAME_COMPONENT_MAX_LEN] = '9' - (i % 10);
    }
    name_str[BIONET_NAME_COMPONENT_MAX_LEN-1] = '.';
    name_str[(2*BIONET_NAME_COMPONENT_MAX_LEN)-1] = 0;
    name = name_str;
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != 0, "failed to accept valid HAB Name %s\n", name);

} END_TEST




// misc invalid inputs
START_TEST (test_libutil_split_hab_name_2) {
    char *hab_type;
    char *hab_id;
    char *name;
    int r;

    name = "HAB-Type.HAB-ID.Node-ID";
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != -1, "failed to reject invalid HAB Name %s\n", name);

    name = ".HAB-ID";
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != -1, "failed to reject invalid HAB Name %s\n", name);

    name = "HAB-Type.";
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != -1, "failed to reject invalid HAB Name %s\n", name);

    name = ".";
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != -1, "failed to reject invalid HAB Name %s\n", name);

    name = "invalid-$.valid";
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != -1, "failed to reject invalid HAB Name %s\n", name);

    name = "valid.invalid-#";
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != -1, "failed to reject invalid HAB Name %s\n", name);

    name = "not so good";
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != -1, "failed to reject invalid HAB Name %s\n", name);

    name = "not_so.good";
    r = bionet_split_hab_name(name, &hab_type, &hab_id);
    fail_if(r != -1, "failed to reject invalid HAB Name %s\n", name);

} END_TEST




void libutil_split_hab_name_suite(Suite *s) {
    TCase *tc = tcase_create("bionet_split_hab_name()");
    suite_add_tcase(s, tc);

    tcase_add_test(tc, test_libutil_split_hab_name_0);
    tcase_add_test(tc, test_libutil_split_hab_name_1);
    tcase_add_test(tc, test_libutil_split_hab_name_2);

    return;
}

