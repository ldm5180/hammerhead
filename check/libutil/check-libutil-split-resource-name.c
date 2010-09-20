
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
#include "check-libutil-hab-tests.h"


// misc NULLs passed in
START_TEST (test_libutil_split_resource_name_0) {
    char *hab_type;
    char *hab_id;
    char *node_id;
    char *resource_id;
    int r;
    char *name;

    r = bionet_split_resource_name(NULL, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != -1, "failed to reject NULL Resource Name\n");

    name = "hello.world.node:resource";
    r = bionet_split_resource_name(name, NULL, &hab_id, &node_id, &resource_id);
    fail_if(r != 0, "failed to accept NULL HAB Type\n");
    fail_if(strcmp("world", hab_id) != 0, "failed to split '%s' hab-id properly with NULL hab-type\n", name);
    fail_if(strcmp("node", node_id) != 0, "failed to split '%s' node-id properly with NULL hab-type\n", name);
    fail_if(strcmp("resource", resource_id) != 0, "failed to split '%s' resource-id properly with NULL hab-type\n", name);

    name = "hello.world.node:resource";
    r = bionet_split_resource_name(name, &hab_type, NULL, &node_id, &resource_id);
    fail_if(r != 0, "failed to accept NULL HAB ID\n");
    fail_if(strcmp("hello", hab_type) != 0, "failed to split '%s' hab-type properly with NULL hab-id\n", name);
    fail_if(strcmp("node", node_id) != 0, "failed to split '%s' node-id properly with NULL hab-id\n", name);
    fail_if(strcmp("resource", resource_id) != 0, "failed to split '%s' resource-id properly with NULL hab-id\n", name);

    name = "hello.world.node:resource";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, NULL, &resource_id);
    fail_if(r != 0, "failed to accept NULL Node ID\n");
    fail_if(strcmp("hello", hab_type) != 0, "failed to split '%s' hab-type properly with NULL node-id\n", name);
    fail_if(strcmp("world", hab_id) != 0, "failed to split '%s' hab-id properly with NULL node-id\n", name);
    fail_if(strcmp("resource", resource_id) != 0, "failed to split '%s' resource-id properly with NULL node-id\n", name);

    name = "hello.world.node:resource";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, NULL);
    fail_if(r != 0, "failed to accept NULL Resource ID\n");
    fail_if(strcmp("hello", hab_type) != 0, "failed to split '%s' hab-type properly with NULL resource-id\n", name);
    fail_if(strcmp("world", hab_id) != 0, "failed to split '%s' hab-id properly with NULL resource-id\n", name);
    fail_if(strcmp("node", node_id) != 0, "failed to split '%s' node-id properly with NULL resource-id\n", name);

} END_TEST




// misc valid inputs
// BIONET_NAME_COMPONENT_MAX_LEN is the max length of a name component, including the terminating NULL
START_TEST (test_libutil_split_resource_name_1) {
    char *hab_type;
    char *hab_id;
    char *node_id;
    char *resource_id;
    char *name;
    int r;

    int i;
    char name_str[BIONET_NAME_COMPONENT_MAX_LEN * 4];

    name = "HAB-Type.HAB-ID.Node-ID:Resource-ID";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != 0, "failed to accept valid Resource Name %s\n", name);
    fail_if(strcmp("HAB-Type", hab_type) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("HAB-ID", hab_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("Node-ID", node_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("Resource-ID", resource_id) != 0, "failed to split '%s'\n", name);

    name = "a.b.c:d";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != 0, "failed to accept valid Resource Name %s\n", name);
    fail_if(strcmp("a", hab_type) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("b", hab_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("c", node_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("d", resource_id) != 0, "failed to split '%s'\n", name);

    name = "*.*.*:*";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != 0, "failed to accept valid HAB Name %s\n", name);
    fail_if(strcmp("*", hab_type) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("*", hab_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("*", node_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("*", resource_id) != 0, "failed to split '%s'\n", name);

    name = "*.hab-id.node-id:resource-id";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != 0, "failed to accept valid Resource Name %s\n", name);
    fail_if(strcmp("*", hab_type) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("hab-id", hab_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("node-id", node_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("resource-id", resource_id) != 0, "failed to split '%s'\n", name);

    name = "hab-type.*.node-id:resource-id";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != 0, "failed to accept valid Resource Name %s\n", name);
    fail_if(strcmp("hab-type", hab_type) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("*", hab_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("node-id", node_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("resource-id", resource_id) != 0, "failed to split '%s'\n", name);

    name = "hab-type.hab-id.*:resource-id";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != 0, "failed to accept valid Resource Name %s\n", name);
    fail_if(strcmp("hab-type", hab_type) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("hab-id", hab_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("*", node_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("resource-id", resource_id) != 0, "failed to split '%s'\n", name);

    name = "hab-type.hab-id.node-id:*";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != 0, "failed to accept valid Resource Name %s\n", name);
    fail_if(strcmp("hab-type", hab_type) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("hab-id", hab_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("node-id", node_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("*", resource_id) != 0, "failed to split '%s'\n", name);

    name = "1.2.3:4";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != 0, "failed to accept valid Resource Name %s\n", name);
    fail_if(strcmp("1", hab_type) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("2", hab_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("3", node_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("4", resource_id) != 0, "failed to split '%s'\n", name);

    // valid chars are [-a-zA-Z0-9]
    name = "-.--.---:----";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != 0, "failed to accept valid Resource Name %s\n", name);
    fail_if(strcmp("-", hab_type) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("--", hab_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("---", node_id) != 0, "failed to split '%s'\n", name);
    fail_if(strcmp("----", resource_id) != 0, "failed to split '%s'\n", name);

    for (i = 0; i < (BIONET_NAME_COMPONENT_MAX_LEN-1); i ++) {
        name_str[i] = 'A' + (i % 26);
        name_str[i+BIONET_NAME_COMPONENT_MAX_LEN] = 'a' + (i % 26);
        name_str[i+(2*BIONET_NAME_COMPONENT_MAX_LEN)] = 'Z' - (i % 26);
        name_str[i+(3*BIONET_NAME_COMPONENT_MAX_LEN)] = 'z' - (i % 26);
    }
    name_str[BIONET_NAME_COMPONENT_MAX_LEN-1] = '.';
    name_str[(2*BIONET_NAME_COMPONENT_MAX_LEN)-1] = '.';
    name_str[(3*BIONET_NAME_COMPONENT_MAX_LEN)-1] = ':';
    name_str[(4*BIONET_NAME_COMPONENT_MAX_LEN)-1] = 0;
    name = name_str;
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != 0, "failed to accept valid Resource Name %s\n", name);

    for (i = 0; i < (BIONET_NAME_COMPONENT_MAX_LEN-1); i ++) {
        name_str[i] = '0' + (i % 10);
        name_str[i+BIONET_NAME_COMPONENT_MAX_LEN] = '9' - (i % 10);
        name_str[i+(2*BIONET_NAME_COMPONENT_MAX_LEN)] = '0' + (i % 10);
        name_str[i+(3*BIONET_NAME_COMPONENT_MAX_LEN)] = '9' - (i % 10);
    }
    name_str[BIONET_NAME_COMPONENT_MAX_LEN-1] = '.';
    name_str[(2*BIONET_NAME_COMPONENT_MAX_LEN)-1] = '.';
    name_str[(3*BIONET_NAME_COMPONENT_MAX_LEN)-1] = ':';
    name_str[(4*BIONET_NAME_COMPONENT_MAX_LEN)-1] = 0;
    name = name_str;
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != 0, "failed to accept valid Resource Name %s\n", name);

} END_TEST




// misc invalid inputs
START_TEST (test_libutil_split_resource_name_2) {
    char *hab_type;
    char *hab_id;
    char *node_id;
    char *resource_id;
    char *name;
    int r;

    name = "HAB-Type.HAB-ID";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != -1, "failed to reject invalid Resource Name %s\n", name);

    name = "HAB-Type.HAB-ID.Node-ID";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != -1, "failed to reject invalid Resource Name %s\n", name);

    name = ".HAB-ID.Node-ID:Resource-ID";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != -1, "failed to reject invalid Resource Name %s\n", name);

    name = "HAB-Type..Node-ID:Resource-ID";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != -1, "failed to reject invalid Resource Name %s\n", name);

    name = "HAB-Type.HAB-ID.:Resource-ID";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != -1, "failed to reject invalid Resource Name %s\n", name);

    name = "HAB-Type.HAB-ID.Node-ID:";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != -1, "failed to reject invalid Resource Name %s\n", name);

    name = "..:";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != -1, "failed to reject invalid Resource Name %s\n", name);

    name = "invalid-$.valid.ok:takeit";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != -1, "failed to reject invalid Resource Name %s\n", name);

    name = "valid.invalid##.ok:takeit";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != -1, "failed to reject invalid Resource Name %s\n", name);

    name = "valid.acceptable.*NotOk:takeit";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != -1, "failed to reject invalid Resource Name %s\n", name);

    name = "not so good";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != -1, "failed to reject invalid Resource Name %s\n", name);

    name = "not_so.good.fine:yes";
    r = bionet_split_resource_name(name, &hab_type, &hab_id, &node_id, &resource_id);
    fail_if(r != -1, "failed to reject invalid Resource Name %s\n", name);

} END_TEST


START_TEST (test_libutil_split_name_components_r_0) {
    char * name = "type.id.node:res";
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

    fail_if(bionet_split_name_components_r(name, hab_type, hab_id, node_id, resource_id),
	    "Failed to split a valid Bionet name: %s", name);

    fail_if(strcmp("type", hab_type),
	    "Failed to get the correct HAB Type: %s", hab_type);

    fail_if(strcmp("id", hab_id),
	    "Failed to get the correct HAB ID: %s", hab_id);

    fail_if(strcmp("node", node_id),
	    "Failed to get the correct Node ID: %s", node_id);

    fail_if(strcmp("res", resource_id),
	    "Failed to get the correct Resource ID: %s", resource_id);
} END_TEST /* bionet_split_name_components_r_0 */


START_TEST (test_libutil_split_name_components_r_1) {
    char * name = "type.id.node";
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

    fail_if(bionet_split_name_components_r(name, hab_type, hab_id, node_id, resource_id),
	    "Failed to split a valid Bionet name: %s", name);

    fail_if(strcmp("type", hab_type),
	    "Failed to get the correct HAB Type: %s", hab_type);

    fail_if(strcmp("id", hab_id),
	    "Failed to get the correct HAB ID: %s", hab_id);

    fail_if(strcmp("node", node_id),
	    "Failed to get the correct Node ID: %s", node_id);

    fail_if(strcmp("", resource_id),
	    "Failed to get the correct Resource ID: %s", resource_id);
} END_TEST /* bionet_split_name_components_r_1 */


START_TEST (test_libutil_split_name_components_r_2) {
    char * name = "type.id";
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

    fail_if(bionet_split_name_components_r(name, hab_type, hab_id, node_id, resource_id),
	    "Failed to split a valid Bionet name: %s", name);

    fail_if(strcmp("type", hab_type),
	    "Failed to get the correct HAB Type: %s", hab_type);

    fail_if(strcmp("id", hab_id),
	    "Failed to get the correct HAB ID: %s", hab_id);

    fail_if(strcmp("", node_id),
	    "Failed to get the correct Node ID: %s", node_id);

    fail_if(strcmp("", resource_id),
	    "Failed to get the correct Resource ID: %s", resource_id);
} END_TEST /* bionet_split_name_components_r_2 */


START_TEST (test_libutil_split_name_components_r_3) {
    char * name = "type";
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

    fail_unless(bionet_split_name_components_r(name, hab_type, hab_id, node_id, resource_id),
	    "Failed to detect only a HAB-Type which is invalid Bionet name: %s", name);

    fail_if(strcmp("", hab_type),
	    "Failed to get the correct HAB Type: %s", hab_type);

    fail_if(strcmp("", hab_id),
	    "Failed to get the correct HAB ID: %s", hab_id);

    fail_if(strcmp("", node_id),
	    "Failed to get the correct Node ID: %s", node_id);

    fail_if(strcmp("", resource_id),
	    "Failed to get the correct Resource ID: %s", resource_id);
} END_TEST /* bionet_split_name_components_r_3 */


START_TEST (test_libutil_split_name_components_r_4) {
    char * name = "";
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

    fail_unless(bionet_split_name_components_r(name, hab_type, hab_id, node_id, resource_id),
	    "Failed to detect invalid blank Bionet name: %s", name);

    fail_if(strcmp("", hab_type),
	    "Failed to get the correct HAB Type: %s", hab_type);

    fail_if(strcmp("", hab_id),
	    "Failed to get the correct HAB ID: %s", hab_id);

    fail_if(strcmp("", node_id),
	    "Failed to get the correct Node ID: %s", node_id);

    fail_if(strcmp("", resource_id),
	    "Failed to get the correct Resource ID: %s", resource_id);
} END_TEST /* bionet_split_name_components_r_4 */


START_TEST (test_libutil_split_name_components_r_5) {
    char * name = "not valid";
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

    fail_unless(bionet_split_name_components_r(name, hab_type, hab_id, node_id, resource_id),
	    "Failed to detect an invalid Bionet name: %s", name);

    fail_if(strcmp("", hab_type),
	    "Failed to get the correct HAB Type: %s", hab_type);

    fail_if(strcmp("", hab_id),
	    "Failed to get the correct HAB ID: %s", hab_id);

    fail_if(strcmp("", node_id),
	    "Failed to get the correct Node ID: %s", node_id);

    fail_if(strcmp("", resource_id),
	    "Failed to get the correct Resource ID: %s", resource_id);
} END_TEST /* bionet_split_name_components_r_5 */


START_TEST (test_libutil_split_name_components_r_6) {
    char * name = NULL;
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

    fail_unless(bionet_split_name_components_r(name, hab_type, hab_id, node_id, resource_id),
	    "Failed to detect a NULL Bionet name: %s", name);

    fail_if(strcmp("", hab_type),
	    "Failed to get the correct HAB Type: %s", hab_type);

    fail_if(strcmp("", hab_id),
	    "Failed to get the correct HAB ID: %s", hab_id);

    fail_if(strcmp("", node_id),
	    "Failed to get the correct Node ID: %s", node_id);

    fail_if(strcmp("", resource_id),
	    "Failed to get the correct Resource ID: %s", resource_id);
} END_TEST /* bionet_split_name_components_r_6 */


START_TEST (test_libutil_split_name_components_r_7) {
    char * name = "foo:bar";
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

    fail_unless(bionet_split_name_components_r(name, hab_type, hab_id, node_id, resource_id),
	    "Failed to detect an invalid Bionet name: %s", name);

    fail_if(strcmp("", hab_type),
	    "Failed to get the correct HAB Type: %s", hab_type);

    fail_if(strcmp("", hab_id),
	    "Failed to get the correct HAB ID: %s", hab_id);

    fail_if(strcmp("", node_id),
	    "Failed to get the correct Node ID: %s", node_id);

    fail_if(strcmp("", resource_id),
	    "Failed to get the correct Resource ID: %s", resource_id);
} END_TEST /* bionet_split_name_components_r_7 */


void libutil_split_resource_name_suite(Suite *s) {
    TCase *tc = tcase_create("Bionet Splitting Resource Names");
    suite_add_tcase(s, tc);

    /* bionet_split_resource_name() */
    tcase_add_test(tc, test_libutil_split_resource_name_0);
    tcase_add_test(tc, test_libutil_split_resource_name_1);
    tcase_add_test(tc, test_libutil_split_resource_name_2);

    /* bionet_split_name_components_r() */
    tcase_add_test(tc, test_libutil_split_name_components_r_0);
    tcase_add_test(tc, test_libutil_split_name_components_r_1);
    tcase_add_test(tc, test_libutil_split_name_components_r_2);
    tcase_add_test(tc, test_libutil_split_name_components_r_3);
    tcase_add_test(tc, test_libutil_split_name_components_r_4);
    tcase_add_test(tc, test_libutil_split_name_components_r_5);
    tcase_add_test(tc, test_libutil_split_name_components_r_6);
    tcase_add_test(tc, test_libutil_split_name_components_r_7);

    return;
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
