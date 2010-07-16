
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



START_TEST (test_libutil_hab_new_and_free_0) {
    bionet_hab_t *hab;

    // make a hab with NULL Type and ID
    hab = bionet_hab_new(NULL, NULL);
    fail_if(hab == NULL, "error creating a new hab");

    // and clean it up
    bionet_hab_free(hab);
} END_TEST

START_TEST (test_libutil_hab_new_and_free_1) {
    bionet_hab_t *hab;

    // make a hab with NULL Type and ID
    hab = bionet_hab_new(NULL, NULL);
    fail_if(hab == NULL, "error creating a new hab");

    // and clean it up
    bionet_hab_free(NULL); //passing in a NULL just to make sure it doesn't crash
} END_TEST


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
 * bionet_hab_add_node(hab, node) twice
 */
START_TEST (test_libutil_hab_add_node_3) {
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

    node = bionet_node_new(hab, "new-node");
    fail_if(NULL == node, "Failed to get a new node: %m\n");

    num = bionet_hab_add_node(hab, node);
    fail_if(0 == num,
	    "Failed to detect adding a node with the same name.");
} END_TEST /* test_libutil_hab_add_node_3 */


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


/*
 * bionet_hab_get_node_by_id(hab, id)
 */
START_TEST (test_libutil_hab_get_node_by_id_0) {
    int num;
    bionet_hab_t * hab;
    bionet_node_t * node;
    bionet_node_t * this_node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "new-node");
    fail_unless(NULL != node, "Failed to get a new node: %m\n");
    this_node = node;

    num = bionet_hab_add_node(hab, node);
    fail_unless(0 == num, 
		"Failed to add node\n");

    node = bionet_hab_get_node_by_id(hab, "new-node");
    fail_if(NULL == node, 
		"Failed to get node new-node.\n");
    fail_if(node != this_node, "Failed to get node 'new-node'\n");
} END_TEST /* test_libutil_hab_get_node_by_id_0 */


/*
 * bionet_hab_get_node_by_id(hab, id)
 */
START_TEST (test_libutil_hab_get_node_by_id_1) {
    int num;
    bionet_hab_t * hab;
    bionet_node_t * node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_hab_get_node_by_id(hab, "new-node");
    fail_unless(NULL == node, 
		"Node should have been NULL because it hasn't been added yet.\n");
} END_TEST /* test_libutil_hab_get_node_by_id_1 */


/*
 * bionet_hab_get_node_by_id(hab, id)
 */
START_TEST (test_libutil_hab_get_node_by_id_2) {
    int num;
    bionet_hab_t * hab;
    bionet_node_t * node;
    bionet_node_t * this_node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "new-node");
    fail_unless(NULL != node, "Failed to get a new node: %m\n");
    this_node = node;

    num = bionet_hab_add_node(hab, node);
    fail_unless(0 == num, 
		"Failed to add node\n");

    node = bionet_hab_get_node_by_id(hab, "fake-node");
    fail_unless(NULL == node, 
		"fake-node does not exist. Nothing should have been found\n");
} END_TEST /* test_libutil_hab_get_node_by_id_2 */


/*
 * bionet_hab_remove_node_by_id(hab, id)
 */
START_TEST (test_libutil_hab_remove_node_by_id_0) {
    int num;
    bionet_hab_t * hab;
    bionet_node_t * node;
    bionet_node_t * this_node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "new-node");
    fail_unless(NULL != node, "Failed to get a new node: %m\n");
    this_node = node;

    num = bionet_hab_add_node(hab, node);
    fail_unless(0 == num, 
		"Failed to add node\n");

    node = bionet_hab_get_node_by_id(hab, "new-node");
    fail_if(NULL == node, "Failed to get node new-node.\n");

    node = bionet_hab_remove_node_by_id(hab, "new-node");
    fail_if (node != this_node, "Failed to remove new-node");

    fail_unless(0 == bionet_hab_get_num_nodes(hab),
		"The only node added has been removed. It should have 0 nodes");
} END_TEST /* test_libutil_hab_remove_node_by_id_0 */


/*
 * bionet_hab_remove_node_by_id(hab, id)
 */
START_TEST (test_libutil_hab_remove_node_by_id_1) {
    int num;
    bionet_hab_t * hab;
    bionet_node_t * node;
    bionet_node_t * this_node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "new-node");
    fail_unless(NULL != node, "Failed to get a new node: %m\n");
    this_node = node;

    num = bionet_hab_add_node(hab, node);
    fail_unless(0 == num, 
		"Failed to add node\n");

    node = bionet_hab_get_node_by_id(hab, "new-node");
    fail_if(NULL == node, "Failed to get node new-node.\n");

    node = bionet_hab_remove_node_by_id(hab, "fake-node");
    fail_if (node != NULL, "fake-node should not have been removed since it doesn't exist\n");

    fail_unless(1 == bionet_hab_get_num_nodes(hab),
		"The only node added hasn't been removed.\n");
} END_TEST /* test_libutil_hab_remove_node_by_id_1 */


/*
 * bionet_hab_remove_node_by_id(hab, id)
 */
START_TEST (test_libutil_hab_remove_node_by_id_2) {
    int num;
    bionet_hab_t * hab;
    bionet_node_t * node;
    bionet_node_t * this_node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_hab_remove_node_by_id(hab, "new-node");
    fail_unless(NULL == node, "No nodes have been added. Removing 'new-node' is impossible.\n");

    fail_unless(0 == bionet_hab_get_num_nodes(hab),
		"No nodes have been added.\n");
} END_TEST /* test_libutil_hab_remove_node_by_id_2 */


/*
 * bionet_hab_remove_node_by_id(hab, id)
 */
START_TEST (test_libutil_hab_remove_node_by_id_3) {
    int num;
    bionet_hab_t * hab;
    bionet_node_t * node;
    bionet_node_t * this_node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "new-node");
    fail_unless(NULL != node, "Failed to get a new node: %m\n");
    this_node = node;

    num = bionet_hab_add_node(hab, node);
    fail_unless(0 == num, 
		"Failed to add node\n");

    node = bionet_hab_get_node_by_id(hab, "new-node");
    fail_if(NULL == node, "Failed to get node new-node.\n");

    node = bionet_hab_remove_node_by_id(NULL, "new-node");
    fail_unless (NULL == node, "NULL hab was passed in. Should have failed.\n");

    fail_unless(1 == bionet_hab_get_num_nodes(hab),
		"The only node added hasn't been removed.\n");
} END_TEST /* test_libutil_hab_remove_node_by_id_3 */


/*
 * bionet_hab_remove_node_by_id(hab, id)
 */
START_TEST (test_libutil_hab_remove_node_by_id_4) {
    int num;
    bionet_hab_t * hab;
    bionet_node_t * node;
    bionet_node_t * this_node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "new-node");
    fail_unless(NULL != node, "Failed to get a new node: %m\n");
    this_node = node;

    num = bionet_hab_add_node(hab, node);
    fail_unless(0 == num, 
		"Failed to add node\n");

    node = bionet_hab_get_node_by_id(hab, "new-node");
    fail_if(NULL == node, "Failed to get node new-node.\n");

    node = bionet_hab_remove_node_by_id(hab, NULL);
    fail_unless (NULL == node, "NULL ID was passed in. Should have failed.\n");

    fail_unless(1 == bionet_hab_get_num_nodes(hab),
		"The only node added hasn't been removed.\n");
} END_TEST /* test_libutil_hab_remove_node_by_id_4 */


/*
 * bionet_hab_remove_all_nodes(hab)
 */
START_TEST (test_libutil_hab_remove_all_nodes_0) {
    int num;
    bionet_hab_t * hab;
    bionet_node_t * node;
    int r;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "new-node0");
    fail_unless(NULL != node, "Failed to get a new node: %m\n");

    num = bionet_hab_add_node(hab, node);
    fail_unless(0 == num, 
		"Failed to add node\n");

    node = bionet_node_new(hab, "new-node1");
    fail_unless(NULL != node, "Failed to get a new node: %m\n");

    num = bionet_hab_add_node(hab, node);
    fail_unless(0 == num, 
		"Failed to add node\n");

    r = bionet_hab_remove_all_nodes(hab);
    fail_unless (0 == r, "Removing all nodes failed.\n");

    fail_unless(0 == bionet_hab_get_num_nodes(hab),
		"The nodes added haven't been removed.\n");

    node = bionet_hab_get_node_by_id(hab, "new-node0");
    fail_unless(NULL == node, "Failed to remove node new-node0.\n");

    node = bionet_hab_get_node_by_id(hab, "new-node1");
    fail_unless(NULL == node, "Failed to remove node new-node1.\n");
} END_TEST /* test_libutil_hab_remove_all_nodes_0 */


/*
 * bionet_hab_remove_all_nodes(hab)
 */
START_TEST (test_libutil_hab_remove_all_nodes_1) {
    int num;
    bionet_hab_t * hab;
    bionet_node_t * node;
    int r;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    r = bionet_hab_remove_all_nodes(hab);
    fail_unless (0 == r, "Removing all nodes failed.\n");

    fail_unless(0 == bionet_hab_get_num_nodes(hab),
		"The nodes added haven't been removed.\n");
} END_TEST /* test_libutil_hab_remove_all_nodes_1 */


/*
 * bionet_hab_remove_all_nodes(hab)
 */
START_TEST (test_libutil_hab_remove_all_nodes_2) {
    int num;
    bionet_hab_t * hab;
    bionet_node_t * node;
    int r;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "new-node");
    fail_unless(NULL != node, "Failed to get a new node: %m\n");

    num = bionet_hab_add_node(hab, node);
    fail_unless(0 == num, 
		"Failed to add node\n");

    r = bionet_hab_remove_all_nodes(NULL);
    fail_if (0 == r, "Passing in a NULL did not cause a failure.\n");

    fail_unless(1 == bionet_hab_get_num_nodes(hab),
		"Removing nodes should have failed when a NULL was passed in.\n");
} END_TEST /* test_libutil_hab_remove_all_nodes_2 */


/*
 * bionet_hab_matches_type_and_id(hab, type, id)
 */
START_TEST (test_libutil_hab_matches_type_and_id_0) {
    bionet_hab_t * hab;

    hab = bionet_hab_new("foo", "bar");
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    fail_unless(bionet_hab_matches_type_and_id(hab, "foo", "bar"),
		"Failed to match exact type and id");
} END_TEST /* test_libutil_hab_matches_type_and_id_0 */


/*
 * bionet_hab_matches_type_and_id(hab, type, id)
 */
START_TEST (test_libutil_hab_matches_type_and_id_1) {
    bionet_hab_t * hab;

    hab = bionet_hab_new("foo", "bar");
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    fail_unless(bionet_hab_matches_type_and_id(hab, "*", "bar"),
		"Failed to match any type and exact id");
} END_TEST /* test_libutil_hab_matches_type_and_id_1 */


/*
 * bionet_hab_matches_type_and_id(hab, type, id)
 */
START_TEST (test_libutil_hab_matches_type_and_id_2) {
    bionet_hab_t * hab;

    hab = bionet_hab_new("foo", "bar");
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    fail_unless(bionet_hab_matches_type_and_id(hab, "foo", "*"),
		"Failed to match exact type and any id");
} END_TEST /* test_libutil_hab_matches_type_and_id_2 */


/*
 * bionet_hab_matches_type_and_id(hab, type, id)
 */
START_TEST (test_libutil_hab_matches_type_and_id_3) {
    bionet_hab_t * hab;

    hab = bionet_hab_new("foo", "bar");
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    fail_unless(bionet_hab_matches_type_and_id(hab, "*", "*"),
		"Failed to match any type and id");
} END_TEST /* test_libutil_hab_matches_type_and_id_3 */


/*
 * bionet_hab_matches_type_and_id(hab, type, id)
 */
START_TEST (test_libutil_hab_matches_type_and_id_4) {
    bionet_hab_t * hab;

    hab = bionet_hab_new("foo", "bar");
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    fail_if(bionet_hab_matches_type_and_id(NULL, "foo", "bar"),
		"matched NULL hab with exact type and id");
} END_TEST /* test_libutil_hab_matches_type_and_id_4 */


/*
 * bionet_hab_matches_type_and_id(hab, type, id)
 */
START_TEST (test_libutil_hab_matches_type_and_id_5) {
    bionet_hab_t * hab;

    hab = bionet_hab_new("foo", "bar");
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    fail_if(bionet_hab_matches_type_and_id(hab, NULL, "bar"),
		"matched hab with NULL type and exact id");
} END_TEST /* test_libutil_hab_matches_type_and_id_5 */


/*
 * bionet_hab_matches_type_and_id(hab, type, id)
 */
START_TEST (test_libutil_hab_matches_type_and_id_6) {
    bionet_hab_t * hab;

    hab = bionet_hab_new("foo", "bar");
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    fail_if(bionet_hab_matches_type_and_id(hab, "foo", NULL),
		"matched hab with exact type and NULL id");
} END_TEST /* test_libutil_hab_matches_type_and_id_6 */


/*
 * bionet_hab_matches_type_and_id(hab, type, id)
 */
START_TEST (test_libutil_hab_matches_type_and_id_7) {
    bionet_hab_t * hab;

    hab = bionet_hab_new("foo", "bar");
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    fail_if(bionet_hab_matches_type_and_id(hab, "bar", "foo"),
		"matched hab with reversed type and id");
} END_TEST /* test_libutil_hab_matches_type_and_id_7 */


/*
 * bionet_hab_set_user_data(hab, user_data)
 * bionet_hab_set_user_data(hab)
 */
START_TEST (test_libutil_hab_set_get_user_data_0) {
    bionet_hab_t * hab;
    char * my_user_data = "my data";
    char * user_data;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    bionet_hab_set_user_data(hab, my_user_data);
    user_data = bionet_hab_get_user_data(hab);
    fail_unless(my_user_data == user_data,
		"User data fetched is not the same pointer as the one set.\n");
} END_TEST /* test_libutil_hab_set_get_user_data_0 */


/*
 * bionet_hab_set_user_data(hab, user_data)
 * bionet_hab_set_user_data(hab)
 */
START_TEST (test_libutil_hab_set_get_user_data_1) {
    bionet_hab_t * hab;
    char * my_user_data = "my data";
    char * user_data;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    bionet_hab_set_user_data(hab, my_user_data);
    bionet_hab_set_user_data(hab, NULL);
    user_data = bionet_hab_get_user_data(hab);
    fail_unless(NULL == user_data,
		"User data fetched is not the same pointer as the one set.\n");
} END_TEST /* test_libutil_hab_set_get_user_data_1 */


/*
 * bionet_hab_set_user_data(hab, user_data)
 * bionet_hab_set_user_data(hab)
 */
START_TEST (test_libutil_hab_set_get_user_data_2) {
    bionet_hab_t * hab;
    char * my_user_data = "my data";
    char * user_data;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    bionet_hab_set_user_data(NULL, my_user_data);
    user_data = bionet_hab_get_user_data(hab);
    fail_unless(NULL == user_data,
		"User data should be NULL because no hab was passed into the set function.\n");
} END_TEST /* test_libutil_hab_set_get_user_data_2 */


/*
 * bionet_hab_set_user_data(hab, user_data)
 * bionet_hab_set_user_data(hab)
 */
START_TEST (test_libutil_hab_set_get_user_data_3) {
    bionet_hab_t * hab;
    char * my_user_data = "my data";
    char * user_data;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    bionet_hab_set_user_data(hab, my_user_data);
    user_data = bionet_hab_get_user_data(NULL);
    fail_unless(NULL == user_data,
		"User data should be NULL because no hab was passed into the get function.\n");
} END_TEST /* test_libutil_hab_set_get_user_data_3 */


void libutil_hab_tests_suite(Suite *s)
{
    TCase *tc = tcase_create("Bionet HAB");
    suite_add_tcase(s, tc);

    /* bionet_hab_new() */
    tcase_add_test(tc, test_libutil_hab_new_0);
    tcase_add_test(tc, test_libutil_hab_new_1);
    tcase_add_test(tc, test_libutil_hab_new_2);
    tcase_add_test(tc, test_libutil_hab_new_3);
    tcase_add_test(tc, test_libutil_hab_new_and_free_0);
    tcase_add_test(tc, test_libutil_hab_new_and_free_1);

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
    tcase_add_test(tc, test_libutil_hab_add_node_3);

    /* bionet_hab_get_node_by_index() */
    tcase_add_test(tc, test_libutil_hab_get_node_by_index_0);
    tcase_add_test(tc, test_libutil_hab_get_node_by_index_1);
    tcase_add_test(tc, test_libutil_hab_get_node_by_index_2);

    /* bionet_hab_get_node_by_id() */
    tcase_add_test(tc, test_libutil_hab_get_node_by_id_0);
    tcase_add_test(tc, test_libutil_hab_get_node_by_id_1);
    tcase_add_test(tc, test_libutil_hab_get_node_by_id_2);

    /* bionet_hab_remove_node_by_id() */
    tcase_add_test(tc, test_libutil_hab_remove_node_by_id_0);
    tcase_add_test(tc, test_libutil_hab_remove_node_by_id_1);
    tcase_add_test(tc, test_libutil_hab_remove_node_by_id_2);
    tcase_add_test(tc, test_libutil_hab_remove_node_by_id_3);
    tcase_add_test(tc, test_libutil_hab_remove_node_by_id_4);

    /* bionet_hab_remove_all_nodes() */
    tcase_add_test(tc, test_libutil_hab_remove_all_nodes_0);
    tcase_add_test(tc, test_libutil_hab_remove_all_nodes_1);
    tcase_add_test(tc, test_libutil_hab_remove_all_nodes_2);

    /* bionet_hab_matches_type_and_id() */
    tcase_add_test(tc, test_libutil_hab_matches_type_and_id_0);
    tcase_add_test(tc, test_libutil_hab_matches_type_and_id_1);
    tcase_add_test(tc, test_libutil_hab_matches_type_and_id_2);
    tcase_add_test(tc, test_libutil_hab_matches_type_and_id_3);
    tcase_add_test(tc, test_libutil_hab_matches_type_and_id_4);
    tcase_add_test(tc, test_libutil_hab_matches_type_and_id_5);
    tcase_add_test(tc, test_libutil_hab_matches_type_and_id_6);
    tcase_add_test(tc, test_libutil_hab_matches_type_and_id_7);

    /* bionet_hab_set_user_data() */
    tcase_add_test(tc, test_libutil_hab_set_get_user_data_0);
    tcase_add_test(tc, test_libutil_hab_set_get_user_data_1);
    tcase_add_test(tc, test_libutil_hab_set_get_user_data_2);
    tcase_add_test(tc, test_libutil_hab_set_get_user_data_3);

    return;
} /* libutil_hab_tests_suite() */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
