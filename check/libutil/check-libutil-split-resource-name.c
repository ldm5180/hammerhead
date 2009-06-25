
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




void libutil_split_resource_name_suite(Suite *s) {
    TCase *tc = tcase_create("bionet_split_resource_name()");
    suite_add_tcase(s, tc);

    tcase_add_test(tc, test_libutil_split_resource_name_0);
    tcase_add_test(tc, test_libutil_split_resource_name_1);
    tcase_add_test(tc, test_libutil_split_resource_name_2);

    return;
}

