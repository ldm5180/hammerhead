
// Copyright (c) 2008-2010, Regents of the University of Colorado.
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
#include "check-libutil-resource-tests.h"


START_TEST (test_libutil_resource_set_str_0) {
    bionet_resource_t *resource;
    int r;

    resource = bionet_resource_new(NULL, BIONET_RESOURCE_DATA_TYPE_STRING, BIONET_RESOURCE_FLAVOR_PARAMETER, "FancyString");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    r = bionet_resource_set_str(resource, "blabber number one", NULL);
    fail_if(r != 0, "failed to set the resource string value the first time\n");

    r = bionet_resource_set_str(resource, "blather number two", NULL);
    fail_if(r != 0, "failed to set the resource string value the second time\n");

    bionet_resource_free(resource);

} END_TEST /* test_libutil_resource_set_str_0 */




void libutil_resource_tests_suite(Suite *s) {
    TCase *tc = tcase_create("Bionet Resource");
    suite_add_tcase(s, tc);

    tcase_add_test(tc, test_libutil_resource_set_str_0);

    return;
} /* libutil_resource_tests_suite */

