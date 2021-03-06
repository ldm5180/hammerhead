
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
#include "check-libutil-node-tests.h"


static void node_destructor(bionet_node_t * node, void * user_data);


/*
 * bionet_node_new(hab, id)
 */
START_TEST (test_libutil_node_new_0) {
    bionet_hab_t * hab = bionet_hab_new(NULL, NULL);
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");
} END_TEST /* test_libutil_node_new_0 */


/*
 * bionet_node_new(hab, id)
 */
START_TEST (test_libutil_node_new_1) {
    bionet_hab_t * hab = bionet_hab_new(NULL, NULL);
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, NULL);
    fail_if (NULL != node, "Failed detect NULL node id passed in.\n");
} END_TEST /* test_libutil_node_new_1 */


/*
 * bionet_node_new(hab, id)
 */
START_TEST (test_libutil_node_new_2) {
    bionet_node_t * node = bionet_node_new(NULL, "node");
    fail_if (NULL == node, "Failed to create a node with a NULL hab ptr\n");
} END_TEST /* test_libutil_node_new_2 */


/*
 * bionet_node_get_name(node)
 */
START_TEST (test_libutil_node_get_name_0) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    const char * node_name = bionet_node_get_name(node);
    fail_unless(0 == strcmp(node_name, "type.id.node"),
		"node name should be type.id.node, but is %s\n", node_name);
} END_TEST /* test_libutil_node_get_name_0 */


/*
 * bionet_node_get_name(node)
 */
START_TEST (test_libutil_node_get_name_1) {
    const char * node_name = bionet_node_get_name(NULL);
    fail_unless(NULL == node_name, "Failed to detect NULL node passed in.");
} END_TEST /* test_libutil_node_get_name_1 */


/*
 * bionet_node_get_name(node)
 */
START_TEST (test_libutil_node_get_name_2) {
    bionet_node_t * node = bionet_node_new(NULL, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    const char * node_name = bionet_node_get_name(node);
    fail_unless(NULL == node_name,
		"Failed to detect node with NULL hab ptr.");
} END_TEST /* test_libutil_node_get_name_2 */


/*
 * bionet_node_get_id(node)
 */
START_TEST (test_libutil_node_get_id_0) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    const char * node_id = bionet_node_get_id(node);
    fail_unless(0 == strcmp(node_id, "node"),
		"node id should be node, but is %s\n", node_id);
} END_TEST /* test_libutil_node_get_id_0 */


/*
 * bionet_node_get_id(node)
 */
START_TEST (test_libutil_node_get_id_1) {
    const char * node_id = bionet_node_get_id(NULL);
    fail_unless(NULL == node_id, "Failed to detect NULL node passed in.");
} END_TEST /* test_libutil_node_get_id_1 */


/*
 * bionet_node_get_id(node)
 */
START_TEST (test_libutil_node_get_id_2) {
    bionet_node_t * node = bionet_node_new(NULL, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    const char * node_id = bionet_node_get_id(node);
    fail_unless(0 == strcmp(node_id, "node"),
		"node id should be node, but is %s and the node's hab was NULL\n", node_id);
} END_TEST /* test_libutil_node_get_id_2 */


/*
 * bionet_node_get_hab(node)
 */
START_TEST (test_libutil_node_get_hab_0) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_hab_t * fetched_hab = bionet_node_get_hab(node);
    fail_unless(fetched_hab == hab,
		"node's hab is not the one used in node creation\n");
} END_TEST /* test_libutil_node_get_hab_0 */


/*
 * bionet_node_get_hab(node)
 */
START_TEST (test_libutil_node_get_hab_1) {
    bionet_node_t * node = bionet_node_new(NULL, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_hab_t * hab = bionet_node_get_hab(node);
    fail_unless(NULL == hab,
		"Failed to get NULL hab from a node with no hab.\n");
} END_TEST /* test_libutil_node_get_hab_1 */


/*
 * bionet_node_get_hab(node)
 */
START_TEST (test_libutil_node_get_hab_2) {
    bionet_hab_t * hab = bionet_node_get_hab(NULL);
    fail_unless(NULL == hab,
		"Failed to detect NULL node passed in.\n");
} END_TEST /* test_libutil_node_get_hab_2 */


/*
 * bionet_node_add_resource(node, resource)
 */
START_TEST (test_libutil_node_add_resource_0) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_resource_t * resource = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_BINARY, 
						       BIONET_RESOURCE_FLAVOR_SENSOR, 
						       "resource");
    fail_if (NULL == resource, "Failed to create resource.\n");

    fail_if (bionet_node_add_resource(node, resource), 
	     "Failed to add resource to node.\n");
} END_TEST /* test_libutil_node_add_resource_0 */


/*
 * bionet_node_add_resource(node, resource)
 */
START_TEST (test_libutil_node_add_resource_1) {
    bionet_node_t * node = bionet_node_new(NULL, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_resource_t * resource = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_BINARY, 
						       BIONET_RESOURCE_FLAVOR_SENSOR, 
						       "resource");
    fail_if (NULL == resource, "Failed to create resource.\n");

    fail_if (bionet_node_add_resource(node, resource), 
	     "Failed to add resource to node that doesn't have a hab.\n");
} END_TEST /* test_libutil_node_add_resource_1 */


/*
 * bionet_node_add_resource(node, resource)
 */
START_TEST (test_libutil_node_add_resource_2) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_resource_t * resource = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_BINARY, 
						       BIONET_RESOURCE_FLAVOR_SENSOR, 
						       "resource");
    fail_if (NULL == resource, "Failed to create resource.\n");

    fail_unless (bionet_node_add_resource(NULL, resource), 
		 "Failed to detect NULL node passed in.\n");
} END_TEST /* test_libutil_node_add_resource_2 */


/*
 * bionet_node_add_resource(node, resource)
 */
START_TEST (test_libutil_node_add_resource_3) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (bionet_node_add_resource(node, NULL), 
		 "Failed to detect NULL resource passed in.\n");
} END_TEST /* test_libutil_node_add_resource_3 */


/*
 * bionet_node_add_resource(node, resource)
 */
START_TEST (test_libutil_node_add_resource_4) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_resource_t * resource = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_BINARY, 
						       BIONET_RESOURCE_FLAVOR_SENSOR, 
						       "resource");
    fail_if (NULL == resource, "Failed to create resource.\n");

    fail_if (bionet_node_add_resource(node, resource), 
	     "Failed to add resource to node.\n");

    resource = bionet_resource_new(node, 
				   BIONET_RESOURCE_DATA_TYPE_BINARY, 
				   BIONET_RESOURCE_FLAVOR_SENSOR, 
				   "resource");
    
    fail_if (NULL == resource, "Failed to create resource.\n");

    fail_unless (bionet_node_add_resource(node, resource), 
		 "Failed to detect a resource with a duplicate ID is being added.\n");
} END_TEST /* test_libutil_node_add_resource_4 */


/*
 * bionet_node_get_num_resources(node)
 */
START_TEST (test_libutil_node_get_num_resources_0) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_resource_t * resource = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_BINARY, 
						       BIONET_RESOURCE_FLAVOR_SENSOR, 
						       "resource");
    fail_if (NULL == resource, "Failed to create resource.\n");

    fail_if (bionet_node_add_resource(node, resource), 
	     "Failed to add resource to node.\n");

    fail_unless(1 == bionet_node_get_num_resources(node), 
		"Failed get correct number of nodes after 1 was added.\n");
} END_TEST /* test_libutil_node_get_num_resources_0 */


/*
 * bionet_node_get_num_resources(node)
 */
START_TEST (test_libutil_node_get_num_resources_1) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless(0 == bionet_node_get_num_resources(node), 
		"Failed get correct number of nodes after none were added.\n");
} END_TEST /* test_libutil_node_get_num_resources_1 */


/*
 * bionet_node_get_num_resources(node)
 */
START_TEST (test_libutil_node_get_num_resources_2) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (bionet_node_add_resource(node, NULL), 
		 "Failed to detect NULL resource being added to node.\n");

    fail_unless(0 == bionet_node_get_num_resources(node), 
		"Failed get correct number of nodes after NULL node was added .\n");
} END_TEST /* test_libutil_node_get_num_resources_2 */


/*
 * bionet_node_get_num_resources(NULL)
 */
START_TEST (test_libutil_node_get_num_resources_3) {
    fail_unless(-1 == bionet_node_get_num_resources(NULL), 
		"Failed to detect NULL node passed in.\n");
} END_TEST /* test_libutil_node_get_num_resources_3 */


/*
 * bionet_node_get_resource_by_index(node, index)
 */
START_TEST (test_libutil_node_get_resource_by_index_0) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_resource_t * resource = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_BINARY, 
						       BIONET_RESOURCE_FLAVOR_SENSOR, 
						       "resource");
    fail_if (NULL == resource, "Failed to create resource.\n");

    fail_if (bionet_node_add_resource(node, resource), 
	     "Failed to add resource to node.\n");

    bionet_resource_t * fetched_resource = bionet_node_get_resource_by_index(node, 0);
    fail_unless(fetched_resource == resource, 
		"Failed to get the resource added.\n");
} END_TEST /* test_libutil_node_get_resource_by_index_0 */


/*
 * bionet_node_get_resource_by_index(node, index)
 */
START_TEST (test_libutil_node_get_resource_by_index_1) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_resource_t * resource = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_BINARY, 
						       BIONET_RESOURCE_FLAVOR_SENSOR, 
						       "resource");
    fail_if (NULL == resource, "Failed to create resource.\n");

    fail_if (bionet_node_add_resource(node, resource), 
	     "Failed to add resource to node.\n");

    bionet_resource_t * fetched_resource = bionet_node_get_resource_by_index(node, 1);
    fail_unless(fetched_resource == NULL, 
		"Failed to get a NULL resource for an index which doesn't exist.\n");
} END_TEST /* test_libutil_node_get_resource_by_index_1 */


/*
 * bionet_node_get_resource_by_index(node, index)
 */
START_TEST (test_libutil_node_get_resource_by_index_2) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_resource_t * fetched_resource = bionet_node_get_resource_by_index(node, 0);
    fail_unless(fetched_resource == NULL, 
		"Failed to get a NULL resource when no resources were added.\n");
} END_TEST /* test_libutil_node_get_resource_by_index_2 */


/*
 * bionet_node_get_resource_by_index(node, index)
 */
START_TEST (test_libutil_node_get_resource_by_index_3) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_resource_t * resource1 = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_BINARY, 
						       BIONET_RESOURCE_FLAVOR_SENSOR, 
						       "resource1");
    fail_if (NULL == resource1, "Failed to create resource.\n");

    fail_if (bionet_node_add_resource(node, resource1), 
	     "Failed to add resource to node.\n");

    bionet_resource_t * resource2 = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_BINARY, 
						       BIONET_RESOURCE_FLAVOR_SENSOR, 
						       "resource2");
    fail_if (NULL == resource2, "Failed to create resource.\n");

    fail_if (bionet_node_add_resource(node, resource2), 
	     "Failed to add resource to node.\n");

    bionet_resource_t * fetched_resource = bionet_node_get_resource_by_index(node, 0);
    fail_unless(fetched_resource == resource1, 
		"Failed to get the first resource added.\n");

    fetched_resource = bionet_node_get_resource_by_index(node, 1);
    fail_unless(fetched_resource == resource2, 
		"Failed to get the second resource added.\n");
} END_TEST /* test_libutil_node_get_resource_by_index_3 */


/*
 * bionet_node_get_resource_by_index(node, index)
 */
START_TEST (test_libutil_node_get_resource_by_index_4) {
    bionet_resource_t * fetched_resource = bionet_node_get_resource_by_index(NULL, 0);
    fail_unless(fetched_resource == NULL, 
		"Failed to detect NULL node passed in.\n");
} END_TEST /* test_libutil_node_get_resource_by_index_4 */


/*
 * bionet_node_get_resource_by_id(node, id)
 */
START_TEST (test_libutil_node_get_resource_by_id_0) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_resource_t * resource = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_BINARY, 
						       BIONET_RESOURCE_FLAVOR_SENSOR, 
						       "resource");
    fail_if (NULL == resource, "Failed to create resource.\n");

    fail_if (bionet_node_add_resource(node, resource), 
	     "Failed to add resource to node.\n");

    bionet_resource_t * fetched_resource = bionet_node_get_resource_by_id(node, "resource");
    fail_unless(fetched_resource == resource, 
		"Failed to get the resource added.\n");
} END_TEST /* test_libutil_node_get_resource_by_id_0 */


/*
 * bionet_node_get_resource_by_id(node, id)
 */
START_TEST (test_libutil_node_get_resource_by_id_1) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_resource_t * resource = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_BINARY, 
						       BIONET_RESOURCE_FLAVOR_SENSOR, 
						       "resource");
    fail_if (NULL == resource, "Failed to create resource.\n");

    fail_if (bionet_node_add_resource(node, resource), 
	     "Failed to add resource to node.\n");

    bionet_resource_t * fetched_resource = bionet_node_get_resource_by_id(node, "resource1");
    fail_unless(fetched_resource == NULL, 
		"Failed to get a NULL resource for an index which doesn't exist.\n");
} END_TEST /* test_libutil_node_get_resource_by_id_1 */


/*
 * bionet_node_get_resource_by_id(node, id)
 */
START_TEST (test_libutil_node_get_resource_by_id_2) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_resource_t * fetched_resource = bionet_node_get_resource_by_id(node, "resource");
    fail_unless(fetched_resource == NULL, 
		"Failed to get a NULL resource when no resources were added.\n");
} END_TEST /* test_libutil_node_get_resource_by_id_2 */


/*
 * bionet_node_get_resource_by_id(node, id)
 */
START_TEST (test_libutil_node_get_resource_by_id_3) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_resource_t * resource1 = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_BINARY, 
						       BIONET_RESOURCE_FLAVOR_SENSOR, 
						       "resource1");
    fail_if (NULL == resource1, "Failed to create resource.\n");

    fail_if (bionet_node_add_resource(node, resource1), 
	     "Failed to add resource to node.\n");

    bionet_resource_t * resource2 = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_BINARY, 
						       BIONET_RESOURCE_FLAVOR_SENSOR, 
						       "resource2");
    fail_if (NULL == resource2, "Failed to create resource.\n");

    fail_if (bionet_node_add_resource(node, resource2), 
	     "Failed to add resource to node.\n");

    bionet_resource_t * fetched_resource = bionet_node_get_resource_by_id(node, "resource1");
    fail_unless(fetched_resource == resource1, 
		"Failed to get the first resource added.\n");

    fetched_resource = bionet_node_get_resource_by_id(node, "resource2");
    fail_unless(fetched_resource == resource2, 
		"Failed to get the second resource added.\n");
} END_TEST /* test_libutil_node_get_resource_by_index_3 */


/*
 * bionet_node_get_resource_by_id(node, id)
 */
START_TEST (test_libutil_node_get_resource_by_id_4) {
    bionet_resource_t * fetched_resource = bionet_node_get_resource_by_id(NULL, "resource");
    fail_unless(fetched_resource == NULL, 
		"Failed to detect NULL node passed in.\n");
} END_TEST /* test_libutil_node_get_resource_by_index_4 */


/*
 * bionet_node_add_stream(node, stream)
 */
START_TEST (test_libutil_node_add_stream_0) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_stream_t * stream = bionet_stream_new(node, 
						 "stream",
						 BIONET_STREAM_DIRECTION_PRODUCER,
						 "streamy");
    fail_if (NULL == stream, "Failed to create stream.\n");

    fail_if (bionet_node_add_stream(node, stream), 
	     "Failed to add stream to node.\n");
} END_TEST /* test_libutil_node_add_stream_0 */


/*
 * bionet_node_add_stream(node, stream)
 */
START_TEST (test_libutil_node_add_stream_1) {
    bionet_node_t * node = bionet_node_new(NULL, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_stream_t * stream = bionet_stream_new(node, 
						 "stream",
						 BIONET_STREAM_DIRECTION_PRODUCER,
						 "streamy");
    fail_if (NULL == stream, "Failed to create stream.\n");

    fail_if (bionet_node_add_stream(node, stream), 
	     "Failed to add stream to node that doesn't have a hab.\n");
} END_TEST /* test_libutil_node_add_stream_1 */


/*
 * bionet_node_add_stream(node, stream)
 */
START_TEST (test_libutil_node_add_stream_2) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_stream_t * stream = bionet_stream_new(node, 
						 "stream",
						 BIONET_STREAM_DIRECTION_PRODUCER,
						 "streamy");

    fail_if (NULL == stream, "Failed to create stream.\n");

    fail_unless (bionet_node_add_stream(NULL, stream), 
		 "Failed to detect NULL node passed in.\n");
} END_TEST /* test_libutil_node_add_stream_2 */


/*
 * bionet_node_add_stream(node, stream)
 */
START_TEST (test_libutil_node_add_stream_3) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (bionet_node_add_stream(node, NULL), 
		 "Failed to detect NULL stream passed in.\n");
} END_TEST /* test_libutil_node_add_stream_3 */


/*
 * bionet_node_add_stream(node, stream)
 */
START_TEST (test_libutil_node_add_stream_4) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_stream_t * stream = bionet_stream_new(node, 
						 "stream",
						 BIONET_STREAM_DIRECTION_PRODUCER,
						 "streamy");

    fail_if (NULL == stream, "Failed to create stream.\n");

    fail_if (bionet_node_add_stream(node, stream), 
	     "Failed to add stream to node.\n");

    stream = bionet_stream_new(node, 
			       "stream",
			       BIONET_STREAM_DIRECTION_PRODUCER,
			       "streamy");
    
    fail_if (NULL == stream, "Failed to create stream.\n");

    fail_unless (bionet_node_add_stream(node, stream), 
		 "Failed to detect a stream with a duplicate ID is being added.\n");
} END_TEST /* test_libutil_node_add_stream_4 */


/*
 * bionet_node_get_num_streams(node)
 */
START_TEST (test_libutil_node_get_num_streams_0) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_stream_t * stream = bionet_stream_new(node, 
						 "stream",
						 BIONET_STREAM_DIRECTION_PRODUCER,
						 "streamy");

    fail_if (NULL == stream, "Failed to create stream.\n");

    fail_if (bionet_node_add_stream(node, stream), 
	     "Failed to add stream to node.\n");

    fail_unless(1 == bionet_node_get_num_streams(node), 
		"Failed get correct number of nodes after 1 was added.\n");
} END_TEST /* test_libutil_node_get_num_streams_0 */


/*
 * bionet_node_get_num_streams(node)
 */
START_TEST (test_libutil_node_get_num_streams_1) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless(0 == bionet_node_get_num_streams(node), 
		"Failed get correct number of nodes after none were added.\n");
} END_TEST /* test_libutil_node_get_num_streams_1 */


/*
 * bionet_node_get_num_streams(node)
 */
START_TEST (test_libutil_node_get_num_streams_2) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (bionet_node_add_stream(node, NULL), 
		 "Failed to detect NULL stream being added to node.\n");

    fail_unless(0 == bionet_node_get_num_streams(node), 
		"Failed get correct number of nodes after NULL node was added .\n");
} END_TEST /* test_libutil_node_get_num_streams_2 */


/*
 * bionet_node_get_num_streams(NULL)
 */
START_TEST (test_libutil_node_get_num_streams_3) {
    fail_unless(-1 == bionet_node_get_num_streams(NULL), 
		"Failed to detect NULL node passed in.\n");
} END_TEST /* test_libutil_node_get_num_streams_3 */


/*
 * bionet_node_get_stream_by_index(node, index)
 */
START_TEST (test_libutil_node_get_stream_by_index_0) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_stream_t * stream = bionet_stream_new(node, 
						 "stream",
						 BIONET_STREAM_DIRECTION_PRODUCER,
						 "streamy");

    fail_if (NULL == stream, "Failed to create stream.\n");

    fail_if (bionet_node_add_stream(node, stream), 
	     "Failed to add stream to node.\n");

    bionet_stream_t * fetched_stream = bionet_node_get_stream_by_index(node, 0);
    fail_unless(fetched_stream == stream, 
		"Failed to get the stream added.\n");
} END_TEST /* test_libutil_node_get_stream_by_index_0 */


/*
 * bionet_node_get_stream_by_index(node, index)
 */
START_TEST (test_libutil_node_get_stream_by_index_1) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_stream_t * stream = bionet_stream_new(node, 
						 "stream",
						 BIONET_STREAM_DIRECTION_PRODUCER,
						 "streamy");

    fail_if (NULL == stream, "Failed to create stream.\n");

    fail_if (bionet_node_add_stream(node, stream), 
	     "Failed to add stream to node.\n");

    bionet_stream_t * fetched_stream = bionet_node_get_stream_by_index(node, 1);
    fail_unless(fetched_stream == NULL, 
		"Failed to get a NULL stream for an index which doesn't exist.\n");
} END_TEST /* test_libutil_node_get_stream_by_index_1 */


/*
 * bionet_node_get_stream_by_index(node, index)
 */
START_TEST (test_libutil_node_get_stream_by_index_2) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_stream_t * fetched_stream = bionet_node_get_stream_by_index(node, 0);
    fail_unless(fetched_stream == NULL, 
		"Failed to get a NULL stream when no streams were added.\n");
} END_TEST /* test_libutil_node_get_stream_by_index_2 */


/*
 * bionet_node_get_stream_by_index(node, index)
 */
START_TEST (test_libutil_node_get_stream_by_index_3) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_stream_t * stream1 = bionet_stream_new(node, 
						 "stream1",
						 BIONET_STREAM_DIRECTION_PRODUCER,
						 "streamy");

    fail_if (NULL == stream1, "Failed to create stream.\n");

    fail_if (bionet_node_add_stream(node, stream1), 
	     "Failed to add stream to node.\n");

    bionet_stream_t * stream2 = bionet_stream_new(node, 
						 "stream2",
						 BIONET_STREAM_DIRECTION_PRODUCER,
						 "streamy");

    fail_if (NULL == stream2, "Failed to create stream.\n");

    fail_if (bionet_node_add_stream(node, stream2), 
	     "Failed to add stream to node.\n");

    bionet_stream_t * fetched_stream = bionet_node_get_stream_by_index(node, 0);
    fail_unless(fetched_stream == stream1, 
		"Failed to get the first stream added.\n");

    fetched_stream = bionet_node_get_stream_by_index(node, 1);
    fail_unless(fetched_stream == stream2, 
		"Failed to get the second stream added.\n");
} END_TEST /* test_libutil_node_get_stream_by_index_3 */


/*
 * bionet_node_get_stream_by_index(node, index)
 */
START_TEST (test_libutil_node_get_stream_by_index_4) {
    bionet_stream_t * fetched_stream = bionet_node_get_stream_by_index(NULL, 0);
    fail_unless(fetched_stream == NULL, 
		"Failed to detect NULL node passed in.\n");
} END_TEST /* test_libutil_node_get_stream_by_index_4 */


/*
 * bionet_node_get_stream_by_id(node, id)
 */
START_TEST (test_libutil_node_get_stream_by_id_0) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_stream_t * stream = bionet_stream_new(node, 
						 "stream",
						 BIONET_STREAM_DIRECTION_PRODUCER,
						 "streamy");

    fail_if (NULL == stream, "Failed to create stream.\n");

    fail_if (bionet_node_add_stream(node, stream), 
	     "Failed to add stream to node.\n");

    bionet_stream_t * fetched_stream = bionet_node_get_stream_by_id(node, "stream");
    fail_unless(fetched_stream == stream, 
		"Failed to get the stream added.\n");
} END_TEST /* test_libutil_node_get_stream_by_id_0 */


/*
 * bionet_node_get_stream_by_id(node, id)
 */
START_TEST (test_libutil_node_get_stream_by_id_1) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_stream_t * stream = bionet_stream_new(node, 
						 "stream",
						 BIONET_STREAM_DIRECTION_PRODUCER,
						 "streamy");

    fail_if (NULL == stream, "Failed to create stream.\n");

    fail_if (bionet_node_add_stream(node, stream), 
	     "Failed to add stream to node.\n");

    bionet_stream_t * fetched_stream = bionet_node_get_stream_by_id(node, "stream1");
    fail_unless(fetched_stream == NULL, 
		"Failed to get a NULL stream for an index which doesn't exist.\n");
} END_TEST /* test_libutil_node_get_stream_by_id_1 */


/*
 * bionet_node_get_stream_by_id(node, id)
 */
START_TEST (test_libutil_node_get_stream_by_id_2) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_stream_t * fetched_stream = bionet_node_get_stream_by_id(node, "stream");
    fail_unless(fetched_stream == NULL, 
		"Failed to get a NULL stream when no streams were added.\n");
} END_TEST /* test_libutil_node_get_stream_by_id_2 */


/*
 * bionet_node_get_stream_by_id(node, id)
 */
START_TEST (test_libutil_node_get_stream_by_id_3) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_stream_t * stream1 = bionet_stream_new(node, 
						 "stream1",
						 BIONET_STREAM_DIRECTION_PRODUCER,
						 "streamy");

    fail_if (NULL == stream1, "Failed to create stream.\n");

    fail_if (bionet_node_add_stream(node, stream1), 
	     "Failed to add stream to node.\n");

    bionet_stream_t * stream2 = bionet_stream_new(node, 
						 "stream2",
						 BIONET_STREAM_DIRECTION_PRODUCER,
						 "streamy");

    fail_if (NULL == stream2, "Failed to create stream.\n");

    fail_if (bionet_node_add_stream(node, stream2), 
	     "Failed to add stream to node.\n");

    bionet_stream_t * fetched_stream = bionet_node_get_stream_by_id(node, "stream1");
    fail_unless(fetched_stream == stream1, 
		"Failed to get the first stream added.\n");

    fetched_stream = bionet_node_get_stream_by_id(node, "stream2");
    fail_unless(fetched_stream == stream2, 
		"Failed to get the second stream added.\n");
} END_TEST /* test_libutil_node_get_stream_by_id_3 */

/*
 * bionet_node_get_stream_by_id(node, id)
 */
START_TEST (test_libutil_node_get_stream_by_id_4) {
    bionet_stream_t * fetched_stream = bionet_node_get_stream_by_id(NULL, "stream");
    fail_unless(fetched_stream == NULL, 
		"Failed to detect NULL node passed in.\n");
} END_TEST /* test_libutil_node_get_stream_by_id_4 */


/*
 * bionet_node_free(node)
 */
START_TEST (test_libutil_node_free_0) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_stream_t * stream = bionet_stream_new(node, 
						 "stream",
						 BIONET_STREAM_DIRECTION_PRODUCER,
						 "streamy");

    fail_if (NULL == stream, "Failed to create stream.\n");

    fail_if (bionet_node_add_stream(node, stream), 
	     "Failed to add stream to node.\n");

    bionet_resource_t * resource = bionet_resource_new(node, 
						       BIONET_RESOURCE_DATA_TYPE_BINARY, 
						       BIONET_RESOURCE_FLAVOR_SENSOR, 
						       "resource");

    fail_if (NULL == resource, "Failed to create resource.\n");

    fail_if (bionet_node_add_resource(node, resource), 
	     "Failed to add resource to node.\n");

    bionet_node_free(node);
} END_TEST /* test_libutil_node_free_0 */


/*
 * bionet_node_free(node)
 */
START_TEST (test_libutil_node_free_1) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_node_free(node);
} END_TEST /* test_libutil_node_free_1 */


/*
 * bionet_node_free(node)
 */
START_TEST (test_libutil_node_free_2) {
    bionet_node_free(NULL);
} END_TEST /* test_libutil_node_free_2 */


/*
 * bionet_node_free(node)
 */
START_TEST (test_libutil_node_free_3) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_if (bionet_hab_add_node(hab, node),
	     "Failed to add node to HAB.");

    bionet_node_free(node);

    bionet_node_t * fetched_node = bionet_hab_get_node_by_index(hab, 0);
    fail_unless (fetched_node == node,
		 "Didn't get dangling node ptr from HAB.");
} END_TEST /* test_libutil_node_free_3 */


/*
 * bionet_node_matches_id(node, id)
 */
START_TEST (test_libutil_node_matches_id_0) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (bionet_node_matches_id(node, "node"),
		 "Failed to match node id %s to 'node'", bionet_node_get_id(node));
} END_TEST /* test_libutil_node_matches_id_0 */


/*
 * bionet_node_matches_id(node, id)
 */
START_TEST (test_libutil_node_matches_id_1) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (bionet_node_matches_id(node, "*"),
		 "Failed to match node id %s to '*'", bionet_node_get_id(node));
} END_TEST /* test_libutil_node_matches_id_1 */


/*
 * bionet_node_matches_id(node, id)
 */
START_TEST (test_libutil_node_matches_id_2) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (0 == bionet_node_matches_id(node, NULL),
		 "Failed to detect NULL node ID passed in.");
} END_TEST /* test_libutil_node_matches_id_2 */


/*
 * bionet_node_matches_id(node, id)
 */
START_TEST (test_libutil_node_matches_id_3) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (0 == bionet_node_matches_id(NULL, "node"),
		 "Failed to detect NULL node passed in.");
} END_TEST /* test_libutil_node_matches_id_3 */


/*
 * bionet_node_matches_habtype_habid_nodeid(node, habtype, habid, nodeid)
 */
START_TEST (test_libutil_node_matches_habtype_habid_nodeid_0) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (bionet_node_matches_habtype_habid_nodeid(node, "type", "id", "node"),
		 "Failed to match node id %s.%s.%s to 'type.id.node'", 
		 bionet_hab_get_type(hab), bionet_hab_get_id(hab), bionet_node_get_id(node));
} END_TEST /* test_libutil_node_matches_habtype_habid_nodeid_0 */


/*
 * bionet_node_matches_habtype_habid_nodeid(node, habtype, habid, nodeid)
 */
START_TEST (test_libutil_node_matches_habtype_habid_nodeid_1) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (bionet_node_matches_habtype_habid_nodeid(node, "*", "*", "*"),
		 "Failed to match node id %s to '*'", bionet_node_get_id(node));
} END_TEST /* test_libutil_node_matches_habtype_habid_nodeid_1 */


/*
 * bionet_node_matches_habtype_habid_nodeid(node, habtype, habid, nodeid)
 */
START_TEST (test_libutil_node_matches_habtype_habid_nodeid_2) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (0 == bionet_node_matches_habtype_habid_nodeid(node, "type", "id", NULL),
		 "Failed to detect NULL node ID passed in.");
} END_TEST /* test_libutil_node_matches_habtype_habid_nodeid_2 */


/*
 * bionet_node_matches_habtype_habid_nodeid(node, habtype, habid, nodeid)
 */
START_TEST (test_libutil_node_matches_habtype_habid_nodeid_3) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (0 == bionet_node_matches_habtype_habid_nodeid(node, "type", NULL, "node"),
		 "Failed to detect NULL HAB ID passed in.");
} END_TEST /* test_libutil_node_matches_habtype_habid_nodeid_3 */


/*
 * bionet_node_matches_habtype_habid_nodeid(node, habtype, habid, nodeid)
 */
START_TEST (test_libutil_node_matches_habtype_habid_nodeid_4) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (0 == bionet_node_matches_habtype_habid_nodeid(node, NULL, "id", "node"),
		 "Failed to detect NULL HAB type passed in.");
} END_TEST /* test_libutil_node_matches_habtype_habid_nodeid_4 */


/*
 * bionet_node_matches_habtype_habid_nodeid(node, habtype, habid, nodeid)
 */
START_TEST (test_libutil_node_matches_habtype_habid_nodeid_5) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (0 == bionet_node_matches_habtype_habid_nodeid(NULL, "type", "id", "node"),
		 "Failed to detect NULL node passed in.");
} END_TEST /* test_libutil_node_matches_habtype_habid_nodeid_5 */


/*
 * bionet_node_matches_habtype_habid_nodeid(node, habtype, habid, nodeid)
 */
START_TEST (test_libutil_node_matches_habtype_habid_nodeid_6) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (bionet_node_matches_habtype_habid_nodeid(node, "type", "*", "*"),
		 "Failed to match node id %s to 'type.*.*'", bionet_node_get_id(node));
} END_TEST /* test_libutil_node_matches_habtype_habid_nodeid_6 */


/*
 * bionet_node_matches_habtype_habid_nodeid(node, habtype, habid, nodeid)
 */
START_TEST (test_libutil_node_matches_habtype_habid_nodeid_7) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (bionet_node_matches_habtype_habid_nodeid(node, "*", "id", "*"),
		 "Failed to match node id %s to '*.id.*'", bionet_node_get_id(node));
} END_TEST /* test_libutil_node_matches_habtype_habid_nodeid_7 */


/*
 * bionet_node_matches_habtype_habid_nodeid(node, habtype, habid, nodeid)
 */
START_TEST (test_libutil_node_matches_habtype_habid_nodeid_8) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    fail_unless (bionet_node_matches_habtype_habid_nodeid(node, "*", "*", "node"),
		 "Failed to match node id %s to '*.*.node'", bionet_node_get_id(node));
} END_TEST /* test_libutil_node_matches_habtype_habid_nodeid_8 */


/*
 * bionet_node_set_user_data(node, user_data)
 * bionet_node_get_user_data(node)
 */
START_TEST (test_libutil_node_get_set_user_data_0) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_node_set_user_data(node, (void *)hab);
    fail_unless(hab == (bionet_hab_t *)bionet_node_get_user_data(node),
		"Failed to get user data which was set.\n");
} END_TEST /* test_libutil_node_get_set_user_data_0 */


/*
 * bionet_node_set_user_data(node, user_data)
 * bionet_node_get_user_data(node)
 */
START_TEST (test_libutil_node_get_set_user_data_1) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_node_set_user_data(node, NULL);
    fail_unless(NULL == bionet_node_get_user_data(node),
		"Failed to get user data which was set to NULL.\n");
} END_TEST /* test_libutil_node_get_set_user_data_1 */


/*
 * bionet_node_set_user_data(node, user_data)
 * bionet_node_get_user_data(node)
 */
START_TEST (test_libutil_node_get_set_user_data_2) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_node_set_user_data(NULL, (void *)hab);
    fail_unless(NULL == bionet_node_get_user_data(node),
		"Failed to get user data which was never.\n");
} END_TEST /* test_libutil_node_get_set_user_data_2 */


/*
 * bionet_node_set_user_data(node, user_data)
 * bionet_node_get_user_data(node)
 */
START_TEST (test_libutil_node_get_set_user_data_3) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_node_set_user_data(node, (void *)hab);
    bionet_node_set_user_data(node, NULL);
    fail_unless(NULL == bionet_node_get_user_data(node),
		"Failed to get user data which was unset.\n");
} END_TEST /* test_libutil_node_get_set_user_data_3 */


/*
 * bionet_node_set_user_data(node, user_data)
 * bionet_node_get_user_data(node)
 */
START_TEST (test_libutil_node_get_set_user_data_4) {
    bionet_hab_t * hab = bionet_hab_new("type", "id");
    fail_if (NULL == hab, "Couldn't even make a hab. How can I possibly move on to nodes?\n");

    bionet_node_t * node = bionet_node_new(hab, "node");
    fail_if (NULL == node, "Failed to create a node.\n");

    bionet_node_set_user_data(NULL, (void *)hab);
    fail_unless(NULL == bionet_node_get_user_data(NULL),
		"Failed to detect NULL node passed into get user data\n");
} END_TEST /* test_libutil_node_get_set_user_data_4 */


/*
 * bionet_node_get_num_events(hab)
 */
START_TEST (test_libutil_node_get_num_events_0) {
    bionet_hab_t * hab;
    bionet_node_t * node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    fail_unless(0 == bionet_node_get_num_events(node),
		"Nothing has happened. There should be 0 events.");
} END_TEST /* test_libutil_node_get_num_events_0 */


/*
 * bionet_node_get_num_events(hab)
 */
START_TEST (test_libutil_node_get_num_events_1) {
    fail_unless(-1 == bionet_node_get_num_events(NULL),
		"NULL Node pointer not detected.");
} END_TEST /* test_libutil_node_get_num_events_1 */


/*
 * bionet_node_get_num_events(hab)
 */
START_TEST (test_libutil_node_get_num_events_2) {
    bionet_hab_t * hab;
    struct timeval tv;
    int num_events;
    bionet_node_t * node;

    hab = bionet_hab_new(NULL, NULL);
    fail_if(NULL == hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    gettimeofday(&tv, NULL);
    bionet_event_t * event = bionet_event_new(&tv, "BDM", BIONET_EVENT_PUBLISHED);
    fail_if(NULL == event, "Failed to create new event.");

    fail_if(bionet_node_add_event(node, event), "Failed to add event to Node.");

    num_events = bionet_node_get_num_events(node);
    fail_unless(1 == num_events,
		"%d is the wrong number of events. There should be 1 events.", num_events);
} END_TEST /* test_libutil_node_get_num_events_2 */


/*
 * bionet_node_get_num_events(node)
 */
START_TEST (test_libutil_node_get_num_events_3) {
    bionet_hab_t * hab;
    bionet_node_t * node;
    int num_events;

    hab = bionet_hab_new(NULL, NULL);
    fail_if(NULL == hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    bionet_event_t * event = bionet_event_new(NULL, "BDM", BIONET_EVENT_PUBLISHED);
    fail_if(NULL == event, "Failed to create new event.");

    fail_if(bionet_node_add_event(node, event), "Failed to add event to Node.");

    num_events = bionet_node_get_num_events(node);
    fail_unless(1 == num_events,
		"%d is the wrong number of events. There should be 1 events.", num_events);

    event = bionet_event_new(NULL, "BDM", BIONET_EVENT_PUBLISHED);
    fail_if(NULL == event, "Failed to create new event.");

    fail_if(bionet_node_add_event(node, event), "Failed to add event to Node.");

    num_events = bionet_node_get_num_events(node);
    fail_unless(2 == num_events,
		"%d is the wrong number of events. There should be 2 events.", num_events);
} END_TEST /* test_libutil_node_get_num_events_3 */


/*
 * bionet_node_get_event_by_index(node)
 */
START_TEST (test_libutil_node_get_event_by_index_0) {
    bionet_hab_t * hab;
    bionet_node_t * node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    fail_unless(NULL == bionet_hab_get_event_by_index(node, 0),
		"Nothing has happened. There should be 0 events.");
} END_TEST /* test_libutil_node_get_event_by_index_0 */


/*
 * bionet_hab_get_event_by_index(hab)
 */
START_TEST (test_libutil_node_get_event_by_index_1) {
    bionet_hab_t * hab;
    bionet_event_t * event;
    bionet_node_t * node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    fail_unless(NULL == bionet_node_get_event_by_index(node, 0),
		"Nothing has happened. There should be 0 events.");

    event = bionet_event_new(NULL, "BDM", BIONET_EVENT_PUBLISHED);
    fail_if(NULL == event, "Failed to create new event.");

    fail_if(bionet_node_add_event(node, event), "Failed to add event to Node.");

    fail_unless(event == bionet_node_get_event_by_index(node, 0),
		"Got a different event than the one added.");
} END_TEST /* test_libutil_node_get_event_by_index_1 */


/*
 * bionet_node_get_event_by_index(node)
 */
START_TEST (test_libutil_node_get_event_by_index_2) {
    bionet_hab_t * hab;
    bionet_event_t * event0;
    bionet_event_t * event1;
    bionet_node_t * node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    fail_unless(NULL == bionet_node_get_event_by_index(node, 0),
		"Nothing has happened. There should be 0 events.");

    event0 = bionet_event_new(NULL, "BDM", BIONET_EVENT_PUBLISHED);
    fail_if(NULL == event0, "Failed to create new event.");

    fail_if(bionet_node_add_event(node, event0), "Failed to add event to Node.");

    event1 = bionet_event_new(NULL, "BDM", BIONET_EVENT_PUBLISHED);
    fail_if(NULL == event1, "Failed to create new event.");

    fail_if(bionet_node_add_event(node, event1), "Failed to add event to Node.");

    fail_unless(event1 == bionet_node_get_event_by_index(node, 1),
		"Got a different event than the second added.");

    fail_unless(event0 == bionet_node_get_event_by_index(node, 0),
		"Got a different event than the first added.");
} END_TEST /* test_libutil_node_get_event_by_index_2 */


/*
 * bionet_hab_get_event_by_index(hab)
 */
START_TEST (test_libutil_node_get_event_by_index_3) {
    fail_unless(NULL == bionet_node_get_event_by_index(NULL, 0),
		"Failed to detect NULL Node passed in.");
} END_TEST /* test_libutil_node_get_event_by_index_3 */


/*
 * bionet_hab_get_event_by_index(hab)
 */
START_TEST (test_libutil_node_get_event_by_index_4) {
    bionet_hab_t * hab;
    bionet_event_t * event;
    bionet_node_t * node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    fail_unless(NULL == bionet_node_get_event_by_index(node, 0),
		"Nothing has happened. There should be 0 events.");

    event = bionet_event_new(NULL, "BDM", BIONET_EVENT_PUBLISHED);
    fail_if(NULL == event, "Failed to create new event.");

    fail_if(bionet_node_add_event(node, event), "Failed to add event to Node.");

    fail_unless(NULL == bionet_node_get_event_by_index(node, 1),
		"There should be 1 events so index 1 is too big.");
} END_TEST /* test_libutil_node_get_event_by_index_4 */


/*
 * bionet_node_add_event(node)
 */
START_TEST (test_libutil_node_add_event_0) {
    bionet_hab_t * hab;
    bionet_event_t * event;
    bionet_node_t * node;
    
    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    event = bionet_event_new(NULL, "BDM", BIONET_EVENT_PUBLISHED);
    fail_if(NULL == event, "Failed to create new event.");

    fail_if(bionet_node_add_event(node, event), "Failed to add event to Node.");

    int num_events = bionet_node_get_num_events(node);
    fail_unless(1 == num_events,
		"1 event added, but Node is reporting %d", num_events);
} END_TEST /* test_libutil_node_add_event_0 */


/*
 * bionet_node_add_event(node)
 */
START_TEST (test_libutil_node_add_event_1) {
    bionet_event_t * event;
    
    event = bionet_event_new(NULL, "BDM", BIONET_EVENT_PUBLISHED);
    fail_if(NULL == event, "Failed to create new event.");

    fail_unless(bionet_node_add_event(NULL, event), 
		"Failed to detect NULL Node passed in.");
} END_TEST /* test_libutil_node_add_event_1 */


/*
 * bionet_node_add_event(node)
 */
START_TEST (test_libutil_node_add_event_2) {
    bionet_hab_t * hab;
    bionet_node_t * node;

    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    fail_unless(bionet_node_add_event(node, NULL), 
		"Failed to detect NULL event passed in.");

    int num_events = bionet_node_get_num_events(node);
    fail_unless(0 == num_events,
		"No event added, but Node is reporting %d", num_events);
} END_TEST /* test_libutil_node_add_event_2 */


/*
 * bionet_node_add_destructor(node)
 */
START_TEST (test_libutil_node_add_destructor_0) {
    bionet_hab_t * hab;
    bionet_node_t * node;
    int called = 0;
    
    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    fail_if (bionet_hab_add_node(hab, node), "Failed to add node to HAB.");

    fail_if(bionet_node_add_destructor(node, node_destructor, &called), "Failed to add destructor to Node.");

    bionet_hab_free(hab);
    
    fail_unless(1 == called, 
		"HAB free'd. Destructor should have been called and counter incremented.");
} END_TEST /* test_libutil_node_add_destructor_0 */


/*
 * bionet_node_add_destructor(node)
 */
START_TEST (test_libutil_node_add_destructor_1) {
    bionet_hab_t * hab;
    bionet_event_t * event;
    bionet_node_t * node;
    int called = 0;
    
    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    fail_if(bionet_node_add_destructor(node, node_destructor, &called), "Failed to add event to HAB.");
    fail_if(bionet_node_add_destructor(node, node_destructor, &called), "Failed to add event to HAB.");

    bionet_node_free(node);
    
    fail_unless(2 == called, 
		"Node free'd. Destructor added twice so should have been called and counter incremented twice.");
} END_TEST /* test_libutil_node_add_destructor_1 */


/*
 * bionet_node_add_destructor(node)
 */
START_TEST (test_libutil_node_add_destructor_2) {
    int called = 0;
    
    fail_unless(bionet_node_add_destructor(NULL, node_destructor, &called), 
		"Failed to detect NULL Node passed in.");
} END_TEST /* test_libutil_node_add_destructor_2 */


/*
 * bionet_node_add_destructor(node)
 */
START_TEST (test_libutil_node_add_destructor_3) {
    bionet_hab_t * hab;
    bionet_node_t * node;
    int called = 0;
    
    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    fail_unless(bionet_node_add_destructor(node, NULL, &called), 
		"Failed to detect NULL destructor passed in.");
} END_TEST /* test_libutil_node_add_destructor_3 */


/*
 * bionet_node_add_destructor(node)
 */
START_TEST (test_libutil_node_add_destructor_4) {
    bionet_hab_t * hab;
    bionet_node_t * node;
    int called = 0;
    
    hab = bionet_hab_new(NULL, NULL);
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    fail_if(bionet_node_add_destructor(node, node_destructor, &called), "Failed to add destructor to Node.");

    bionet_node_free(node);
    
    fail_unless(1 == called, 
		"Node free'd. Destructor should have been called and counter incremented.");
} END_TEST /* test_libutil_node_add_destructor_4 */


void libutil_node_tests_suite(Suite *s)
{
    TCase *tc = tcase_create("Bionet Node");
    suite_add_tcase(s, tc);

    /* bionet_node_new() */
    tcase_add_test(tc, test_libutil_node_new_0);
    tcase_add_test(tc, test_libutil_node_new_1);
    tcase_add_test(tc, test_libutil_node_new_2);

    /* bionet_node_get_name() */
    tcase_add_test(tc, test_libutil_node_get_name_0);
    tcase_add_test(tc, test_libutil_node_get_name_1);
    tcase_add_test(tc, test_libutil_node_get_name_2);

    /* bionet_node_get_id() */
    tcase_add_test(tc, test_libutil_node_get_id_0);
    tcase_add_test(tc, test_libutil_node_get_id_1);
    tcase_add_test(tc, test_libutil_node_get_id_2);

    /* bionet_node_get_hab() */
    tcase_add_test(tc, test_libutil_node_get_hab_0);
    tcase_add_test(tc, test_libutil_node_get_hab_1);
    tcase_add_test(tc, test_libutil_node_get_hab_2);

    /* bionet_node_add_resource() */
    tcase_add_test(tc, test_libutil_node_add_resource_0);
    tcase_add_test(tc, test_libutil_node_add_resource_1);
    tcase_add_test(tc, test_libutil_node_add_resource_2);
    tcase_add_test(tc, test_libutil_node_add_resource_3);
    tcase_add_test(tc, test_libutil_node_add_resource_4);

    /* bionet_node_get_num_resources() */
    tcase_add_test(tc, test_libutil_node_get_num_resources_0);
    tcase_add_test(tc, test_libutil_node_get_num_resources_1);
    tcase_add_test(tc, test_libutil_node_get_num_resources_2);
    tcase_add_test(tc, test_libutil_node_get_num_resources_3);

    /* bionet_node_get_resource_by_index() */
    tcase_add_test(tc, test_libutil_node_get_resource_by_index_0);
    tcase_add_test(tc, test_libutil_node_get_resource_by_index_1);
    tcase_add_test(tc, test_libutil_node_get_resource_by_index_2);
    tcase_add_test(tc, test_libutil_node_get_resource_by_index_3);
    tcase_add_test(tc, test_libutil_node_get_resource_by_index_4);

    /* bionet_node_get_resource_by_id() */
    tcase_add_test(tc, test_libutil_node_get_resource_by_id_0);
    tcase_add_test(tc, test_libutil_node_get_resource_by_id_1);
    tcase_add_test(tc, test_libutil_node_get_resource_by_id_2);
    tcase_add_test(tc, test_libutil_node_get_resource_by_id_3);
    tcase_add_test(tc, test_libutil_node_get_resource_by_id_4);

    /* bionet_node_add_stream() */
    tcase_add_test(tc, test_libutil_node_add_stream_0);
    tcase_add_test(tc, test_libutil_node_add_stream_1);
    tcase_add_test(tc, test_libutil_node_add_stream_2);
    tcase_add_test(tc, test_libutil_node_add_stream_3);
    tcase_add_test(tc, test_libutil_node_add_stream_4);

    /* bionet_node_get_num_streams() */
    tcase_add_test(tc, test_libutil_node_get_num_streams_0);
    tcase_add_test(tc, test_libutil_node_get_num_streams_1);
    tcase_add_test(tc, test_libutil_node_get_num_streams_2);
    tcase_add_test(tc, test_libutil_node_get_num_streams_3);

    /* bionet_node_get_stream_by_index() */
    tcase_add_test(tc, test_libutil_node_get_stream_by_index_0);
    tcase_add_test(tc, test_libutil_node_get_stream_by_index_1);
    tcase_add_test(tc, test_libutil_node_get_stream_by_index_2);
    tcase_add_test(tc, test_libutil_node_get_stream_by_index_3);
    tcase_add_test(tc, test_libutil_node_get_stream_by_index_4);

    /* bionet_node_get_stream_by_id() */
    tcase_add_test(tc, test_libutil_node_get_stream_by_id_0);
    tcase_add_test(tc, test_libutil_node_get_stream_by_id_1);
    tcase_add_test(tc, test_libutil_node_get_stream_by_id_2);
    tcase_add_test(tc, test_libutil_node_get_stream_by_id_3);
    tcase_add_test(tc, test_libutil_node_get_stream_by_id_4);

    /* bionet_node_free() */
    tcase_add_test(tc, test_libutil_node_free_0);
    tcase_add_test(tc, test_libutil_node_free_1);
    tcase_add_test(tc, test_libutil_node_free_2);
    tcase_add_test(tc, test_libutil_node_free_3);

    /* bionet_node_matches_id() */
    tcase_add_test(tc, test_libutil_node_matches_id_0);
    tcase_add_test(tc, test_libutil_node_matches_id_1);
    tcase_add_test(tc, test_libutil_node_matches_id_2);
    tcase_add_test(tc, test_libutil_node_matches_id_3);

    /* bionet_node_matches_habtype_habid_nodeid() */
    tcase_add_test(tc, test_libutil_node_matches_habtype_habid_nodeid_0);
    tcase_add_test(tc, test_libutil_node_matches_habtype_habid_nodeid_1);
    tcase_add_test(tc, test_libutil_node_matches_habtype_habid_nodeid_2);
    tcase_add_test(tc, test_libutil_node_matches_habtype_habid_nodeid_3);
    tcase_add_test(tc, test_libutil_node_matches_habtype_habid_nodeid_4);
    tcase_add_test(tc, test_libutil_node_matches_habtype_habid_nodeid_5);
    tcase_add_test(tc, test_libutil_node_matches_habtype_habid_nodeid_6);
    tcase_add_test(tc, test_libutil_node_matches_habtype_habid_nodeid_7);
    tcase_add_test(tc, test_libutil_node_matches_habtype_habid_nodeid_8);

    /* bionet_node_get_user_data() */
    /* bionet_node_set_user_data() */
    tcase_add_test(tc, test_libutil_node_get_set_user_data_0);
    tcase_add_test(tc, test_libutil_node_get_set_user_data_1);
    tcase_add_test(tc, test_libutil_node_get_set_user_data_2);
    tcase_add_test(tc, test_libutil_node_get_set_user_data_3);
    tcase_add_test(tc, test_libutil_node_get_set_user_data_4);

    /* bionet_node_get_num_events() */
    tcase_add_test(tc, test_libutil_node_get_num_events_0);
    tcase_add_test(tc, test_libutil_node_get_num_events_1);
    tcase_add_test(tc, test_libutil_node_get_num_events_2);
    tcase_add_test(tc, test_libutil_node_get_num_events_3);

    /* bionet_node_get_event_by_index() */
    tcase_add_test(tc, test_libutil_node_get_event_by_index_0);
    tcase_add_test(tc, test_libutil_node_get_event_by_index_1);
    tcase_add_test(tc, test_libutil_node_get_event_by_index_2);
    tcase_add_test(tc, test_libutil_node_get_event_by_index_3);
    tcase_add_test(tc, test_libutil_node_get_event_by_index_4);

    /* bionet_node_add_event() */
    tcase_add_test(tc, test_libutil_node_add_event_0);
    tcase_add_test(tc, test_libutil_node_add_event_1);
    tcase_add_test(tc, test_libutil_node_add_event_2);

    /* bionet_node_add_destructor*/
    tcase_add_test(tc, test_libutil_node_add_destructor_0);
    tcase_add_test(tc, test_libutil_node_add_destructor_1);
    tcase_add_test(tc, test_libutil_node_add_destructor_2);
    tcase_add_test(tc, test_libutil_node_add_destructor_3);
    tcase_add_test(tc, test_libutil_node_add_destructor_4);

    return;
} /* libutil_node_tests_suite() */

static void node_destructor(bionet_node_t * node, void * user_data) {
    int * called = (int *)user_data;
    *called = *called + 1;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
