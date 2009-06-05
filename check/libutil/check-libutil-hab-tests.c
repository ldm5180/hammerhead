
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

/*
 * bionet_hab_new(type, NULL)
 * bionet_hab_get_name(hab)
 */
START_TEST (test_libutil_hab_new_0) {
    bionet_hab_t * hab;
    const char * hab_type;
    const char * hab_id;
    char hostname[250];
    char * tmp;

    const char * mytype = "test123";

    hab = bionet_hab_new(mytype, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");
    
    hab_type = bionet_hab_get_type(hab);
    hab_id = bionet_hab_get_id(hab);

    fail_if(strcmp(hab_type, mytype), 
	    "HAB-Type should be %s, but is %s", 
	    mytype, hab_type);

    int r;
    r = gethostname(hostname, sizeof(hostname));
    fail_if(r < 0, "error getting hostname: %m");

    for (tmp = hostname; *tmp != '\0'; tmp ++) {
        if (!isalnum(*tmp) && *tmp != '-') {
            *tmp = '-';
        }
    }


    fail_if(strcmp(hab_id, hostname), "HAB-ID should be %s, but is %s",
	    hostname, hab_id);
} END_TEST /* test_libutil_hab_new_0 */


/*
 * bionet_hab_new(NULL, NULL)
 * bionet_hab_get_name(hab)
 */
START_TEST (test_libutil_hab_new_1) {
    bionet_hab_t * hab;
    const char * hab_type;
    const char * hab_id;
    char * tmp;

    const char * myid = "test123";
    char program_name[500];
    int fd, r;
 
    hab = bionet_hab_new(NULL, myid);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");
    
    hab_type = bionet_hab_get_type(hab);
    hab_id = bionet_hab_get_id(hab);

#ifdef LINUX
    fd = open("/proc/self/cmdline", O_RDONLY);
    fail_if(fd < 0,
	    "error opening /proc/self/cmdline (%m)\n");

    r = read(fd, program_name, sizeof(program_name) - 1);
    close(fd);
    fail_if (r <= 0,
	     "error reading /proc/self/cmdline (%m)\n");

    while ((tmp = memchr(program_name, '/', strlen(program_name))) != NULL) {
        int new_len = strlen(tmp+1);
        memmove(program_name, tmp+1, new_len);
        program_name[new_len] = '\0';
    }

    for (tmp = program_name; *tmp != '\0'; tmp ++) {
        if (!isalnum(*tmp) && *tmp != '-') {
            *tmp = '-';
        }
    }

    fail_if(strcmp(hab_type, program_name), 
	    "HAB-Type should be %s, but is %s\n", 
	    program_name, hab_type);

    fail_if(strcmp(myid, hab_id),
	    "HAB-ID should be %s, but it is %s\n",
	    myid, hab_id);
#endif
} END_TEST /* test_libutil_hab_new_1 */




/*
 * bionet_hab_new(type, NULL)
 * bionet_hab_get_name(hab)
 */
START_TEST (test_libutil_hab_new_2) {
    bionet_hab_t * hab;
    const char * hab_type;
    const char * hab_id;
    char hostname[250];
    char * tmp;

    char program_name[500];
    int fd, r;
 
    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");
    
    hab_type = bionet_hab_get_type(hab);
    hab_id = bionet_hab_get_id(hab);

#ifdef LINUX
    fd = open("/proc/self/cmdline", O_RDONLY);
    fail_if(fd < 0,
	    "error opening /proc/self/cmdline (%m)\n");

    r = read(fd, program_name, sizeof(program_name) - 1);
    close(fd);
    fail_if (r <= 0,
	     "error reading /proc/self/cmdline (%m)\n");

    while ((tmp = memchr(program_name, '/', strlen(program_name))) != NULL) {
        int new_len = strlen(tmp+1);
        memmove(program_name, tmp+1, new_len);
        program_name[new_len] = '\0';
    }

    for (tmp = program_name; *tmp != '\0'; tmp ++) {
        if (!isalnum(*tmp) && *tmp != '-') {
            *tmp = '-';
        }
    }

    fail_if(strcmp(hab_type, program_name), 
	    "HAB-Type should be %s, but is %s\n", 
	    program_name, hab_type);

    r = gethostname(hostname, sizeof(hostname));
    fail_if(r < 0, "error getting hostname: %m");

    for (tmp = hostname; *tmp != '\0'; tmp ++) {
        if (!isalnum(*tmp) && *tmp != '-') {
            *tmp = '-';
        }
    }

    fail_if(strcmp(hostname, hab_id),
	    "HAB-ID should be %s, but it is %s\n",
	    hostname, hab_id);
#endif
} END_TEST /* test_libutil_hab_new_2 */


/*
 * bionet_hab_new(type, id)
 * bionet_hab_get_name(hab)
 */
START_TEST (test_libutil_hab_new_3) {
    bionet_hab_t * hab;
    const char * hab_type;
    const char * hab_id;
    const char * mytype = "test123";

    hab = bionet_hab_new(mytype, mytype);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");
    
    hab_type = bionet_hab_get_type(hab);
    hab_id = bionet_hab_get_id(hab);

    fail_if(strcmp(hab_type, mytype), 
	    "HAB-Type should be %s, but is %s", 
	    mytype, hab_type);

    fail_if(strcmp(hab_id, mytype), 
	    "HAB-ID should be %s, but is %s",
	    mytype, hab_id);
} END_TEST /* test_libutil_hab_new_0 */


/*
 * bionet_hab_get_name(NULL)
 */
START_TEST (test_libutil_hab_get_name_0) {
    const char * hab_name;

    hab_name = bionet_hab_get_name(NULL);
    fail_unless(NULL == hab_name, 
		"NULL HAB does not have a name.");
} END_TEST /* test_libutil_hab_get_name_0 */


/*
 * bionet_hab_get_name(name)
 */
START_TEST (test_libutil_hab_get_name_1) {
    bionet_hab_t * hab;
    const char * hab_name;

    const char * mytype = "test123";
    const char * myid = "test345";
    const char * myname = "test123.test345";

    hab = bionet_hab_new(mytype, myid);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");
    
    hab_name = bionet_hab_get_name(hab);

    fail_if(strcmp(hab_name, myname), 
	    "HAB Name should be %s, but is %s", 
	    myname, hab_name);
} END_TEST /* test_libutil_hab_get_name_1 */


/*
 * bionet_hab_get_type(NULL)
 */
START_TEST (test_libutil_hab_get_type_0) {
    const char * hab_type;

    hab_type = bionet_hab_get_type(NULL);
    fail_unless(NULL == hab_type, 
		"NULL HAB does not have a type.\n");
} END_TEST /* test_libutil_hab_get_type_0 */


/*
 * bionet_hab_get_id(NULL)
 */
START_TEST (test_libutil_hab_get_id_0) {
    const char * hab_id;

    hab_id = bionet_hab_get_id(NULL);
    fail_unless(NULL == hab_id, 
		"NULL HAB does not have a id.\n");
} END_TEST /* test_libutil_hab_get_id_0 */


/*
 * bionet_hab_get_num_nodes(NULL)
 */
START_TEST (test_libutil_hab_get_num_nodes_0) {
    int num;

    num = bionet_hab_get_num_nodes(NULL);
    fail_unless(-1 == num, 
		"Failed to detect NULL HAB\n");
} END_TEST /* test_libutil_hab_get_num_nodes_0 */


/*
 * bionet_hab_get_num_nodes(hab)
 */
START_TEST (test_libutil_hab_get_num_nodes_1) {
    int num;
    bionet_hab_t * hab;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    num = bionet_hab_get_num_nodes(hab);
    fail_unless(0 == num, 
		"Failed to detect NULL HAB\n");
} END_TEST /* test_libutil_hab_get_num_nodes_1 */


/*
 * bionet_hab_get_num_nodes(hab)
 */
START_TEST (test_libutil_hab_get_num_nodes_2) {
    int num;
    bionet_hab_t * hab;
    bionet_node_t * node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "new-node");
    fail_unless(NULL != node, "Failed to get a new node: %m\n");

    num = bionet_hab_add_node(hab, node);
    fail_unless(0 == num, 
		"Failed to add node\n");

    num = bionet_hab_get_num_nodes(hab);
    fail_unless(1 == num, 
		"Failed to find 1 node\n");
} END_TEST /* test_libutil_hab_get_num_nodes_2 */


/*
 * bionet_hab_add_node(NULL, NULL)
 */
START_TEST (test_libutil_hab_add_node_0) {
    int num;

    num = bionet_hab_add_node(NULL, NULL);
    fail_unless(-1 == num, 
		"Failed to detect NULL HAB\n");
} END_TEST /* test_libutil_hab_add_node_0 */


/*
 * bionet_hab_add_node(hab, NULL)
 */
START_TEST (test_libutil_hab_add_node_1) {
    int num;
    bionet_hab_t * hab;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    num = bionet_hab_add_node(hab, NULL);
    fail_unless(-1 == num, 
		"Failed to detect NULL node\n");
} END_TEST /* test_libutil_hab_add_node_1 */


/*
 * bionet_hab_add_node(hab, node)
 */
START_TEST (test_libutil_hab_add_node_2) {
    int num;
    bionet_hab_t * hab;
    bionet_node_t * node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "new-node");
    fail_unless(NULL != node, "Failed to get a new node: %m\n");

    num = bionet_hab_add_node(hab, node);
    fail_unless(0 == num, 
		"Failed to add node\n");
} END_TEST /* test_libutil_hab_add_node_2 */


/*
 * bionet_hab_get_node_by_index(NULL, 0)
 */
START_TEST (test_libutil_hab_get_node_by_index_0) {
    bionet_node_t * node;

    node = bionet_hab_get_node_by_index(NULL, 0);
    fail_unless(NULL == node, 
		"Failed to detect NULL HAB\n");
} END_TEST /* test_libutil_hab_get_node_by_index_0 */


/*
 * bionet_hab_get_node_by_index(hab, 0)
 */
START_TEST (test_libutil_hab_get_node_by_index_1) {
    bionet_node_t * node;
    bionet_hab_t * hab;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_hab_get_node_by_index(hab, 0);
    fail_unless(NULL == node, 
		"Failed to detect lack of nodes\n");
} END_TEST /* test_libutil_hab_get_node_by_index_1 */


/*
 * bionet_hab_get_node_by_index(hab, node)
 */
START_TEST (test_libutil_hab_get_node_by_index_2) {
    int num;
    bionet_hab_t * hab;
    bionet_node_t * node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "new-node");
    fail_unless(NULL != node, "Failed to get a new node: %m\n");

    num = bionet_hab_add_node(hab, node);
    fail_unless(0 == num, 
		"Failed to add node\n");

    node = bionet_hab_get_node_by_index(hab, 0);
    fail_if(NULL == node, 
		"Failed to get node 0.\n");
} END_TEST /* test_libutil_hab_get_node_by_index_2 */


void libutil_hab_tests_suite(Suite *s)
{
    TCase *tc = tcase_create("bionet_hab_new()");
    suite_add_tcase(s, tc);

    /* bionet_hab_new() */
    tcase_add_test(tc, test_libutil_hab_new_0);
    tcase_add_test(tc, test_libutil_hab_new_1);
    tcase_add_test(tc, test_libutil_hab_new_2);
    tcase_add_test(tc, test_libutil_hab_new_3);

    /* bionet_hab_get_name() */
    tcase_add_test(tc, test_libutil_hab_get_name_0);
    tcase_add_test(tc, test_libutil_hab_get_name_1);

    /* bionet_hab_get_type() */
    tcase_add_test(tc, test_libutil_hab_get_type_0);

    /* bionet_hab_get_id() */
    tcase_add_test(tc, test_libutil_hab_get_id_0);

    /* bionet_hab_get_num_nodes() */
    tcase_add_test(tc, test_libutil_hab_get_num_nodes_0);
    tcase_add_test(tc, test_libutil_hab_get_num_nodes_1);
    tcase_add_test(tc, test_libutil_hab_get_num_nodes_2);

    /* bionet_hab_add_node() */
    tcase_add_test(tc, test_libutil_hab_add_node_0);
    tcase_add_test(tc, test_libutil_hab_add_node_1);
    tcase_add_test(tc, test_libutil_hab_add_node_2);

    /* bionet_hab_get_node_by_index() */
    tcase_add_test(tc, test_libutil_hab_get_node_by_index_0);
    tcase_add_test(tc, test_libutil_hab_get_node_by_index_1);
    tcase_add_test(tc, test_libutil_hab_get_node_by_index_2);

    return;
} /* libutil_hab_tests_suite() */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
