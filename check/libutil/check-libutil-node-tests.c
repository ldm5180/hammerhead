
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
} END_TEST /* test_libutil_node_get_stream_by_index_3 */


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
 * bionet_node_get_stream_by_id(node, id)
 */
START_TEST (test_libutil_node_get_stream_by_id_4) {
    bionet_stream_t * fetched_stream = bionet_node_get_stream_by_id(NULL, "stream");
    fail_unless(fetched_stream == NULL, 
		"Failed to detect NULL node passed in.\n");
} END_TEST /* test_libutil_node_get_stream_by_index_4 */


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

    return;
} /* libutil_node_tests_suite() */



// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
