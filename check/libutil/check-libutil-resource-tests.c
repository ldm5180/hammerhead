
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


START_TEST (test_libutil_resource_new_0) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");
} END_TEST /* test_libutil_resource_new_0 */


START_TEST (test_libutil_resource_new_1) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to resources?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Couldn't even make a node. How can I possibly move on to resources?\n");

    bionet_resource_t * resource = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_STRING, 
						       BIONET_RESOURCE_FLAVOR_PARAMETER, 
						       "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");
} END_TEST /* test_libutil_resource_new_1 */


START_TEST (test_libutil_resource_new_2) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to resources?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Couldn't even make a node. How can I possibly move on to resources?\n");

    bionet_resource_t * resource = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_INVALID, 
						       BIONET_RESOURCE_FLAVOR_PARAMETER, 
						       "resource");
    fail_unless(resource == NULL, "failed to detect invalid data type");
} END_TEST /* test_libutil_resource_new_2 */


START_TEST (test_libutil_resource_new_3) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to resources?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Couldn't even make a node. How can I possibly move on to resources?\n");

    bionet_resource_t * resource = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_STRING, 
						       BIONET_RESOURCE_FLAVOR_INVALID, 
						       "resource");
    fail_unless(resource == NULL, "failed to detect invalid flavor");
} END_TEST /* test_libutil_resource_new_3 */


START_TEST (test_libutil_resource_new_4) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to resources?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Couldn't even make a node. How can I possibly move on to resources?\n");

    bionet_resource_t * resource = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_STRING, 
						       BIONET_RESOURCE_FLAVOR_INVALID, 
						       NULL);
    fail_unless(resource == NULL, "failed to detect ID");
} END_TEST /* test_libutil_resource_new_4 */


START_TEST (test_libutil_resource_free_0) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    bionet_resource_free(resource);
} END_TEST /* test_libutil_resource_free_0 */


START_TEST (test_libutil_resource_free_1) {
    bionet_resource_free(NULL);
} END_TEST /* test_libutil_resource_free_1 */


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

    /* bionet_resource_new() */
    tcase_add_test(tc, test_libutil_resource_new_0);
    tcase_add_test(tc, test_libutil_resource_new_1);
    tcase_add_test(tc, test_libutil_resource_new_2);
    tcase_add_test(tc, test_libutil_resource_new_3);
    tcase_add_test(tc, test_libutil_resource_new_4);

    /* bionet_resource_free() */
    tcase_add_test(tc, test_libutil_resource_free_0);
    tcase_add_test(tc, test_libutil_resource_free_1);

    /* bionet_resource_set_str() */
    tcase_add_test(tc, test_libutil_resource_set_str_0);

    return;
} /* libutil_resource_tests_suite */

