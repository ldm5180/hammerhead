
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


START_TEST (test_libutil_resource_get_name_0) {
    bionet_resource_t * resource;
    char * resname;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(NULL == bionet_resource_get_name(resource),
		"Resource has no node pointer. How can it have a name?");
} END_TEST /* test_libutil_resource_get_name_0 */


START_TEST (test_libutil_resource_get_name_1) {
    bionet_node_t * node;
    bionet_resource_t * resource;
    char * resname;

    node = bionet_node_new(NULL, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    resource = bionet_resource_new(node, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(NULL == bionet_resource_get_name(resource),
		"Resource's node has no HAB pointer. How can it have a name?");
} END_TEST /* test_libutil_resource_get_name_1 */


START_TEST (test_libutil_resource_get_name_2) {
    bionet_hab_t * hab;
    bionet_node_t * node;
    bionet_resource_t * resource;
    char * resname;

    hab = bionet_hab_new("foo", "bar");
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    resource = bionet_resource_new(node, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if(strcmp("foo.bar.node:resource", bionet_resource_get_name(resource)),
		   "Incorrect resource name %s", bionet_resource_get_name(resource));
} END_TEST /* test_libutil_resource_get_name_2 */


START_TEST (test_libutil_resource_get_name_3) {
    fail_if(NULL != bionet_resource_get_name(NULL),
	    "Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_name_3 */


START_TEST (test_libutil_resource_get_local_name_0) {
    bionet_resource_t * resource;
    char * resname;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(NULL == bionet_resource_get_local_name(resource),
		"Resource has no node pointer. How can it have a local name?");
} END_TEST /* test_libutil_resource_get_local_name_0 */


START_TEST (test_libutil_resource_get_local_name_1) {
    bionet_node_t * node;
    bionet_resource_t * resource;
    char * resname;

    node = bionet_node_new(NULL, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    resource = bionet_resource_new(node, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if(strcmp("node:resource", bionet_resource_get_local_name(resource)),
		   "Incorrect local resource name %s", bionet_resource_get_local_name(resource));
} END_TEST /* test_libutil_resource_get_local_name_1 */


START_TEST (test_libutil_resource_get_local_name_2) {
    bionet_hab_t * hab;
    bionet_node_t * node;
    bionet_resource_t * resource;
    char * resname;

    hab = bionet_hab_new("foo", "bar");
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    resource = bionet_resource_new(node, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if(strcmp("node:resource", bionet_resource_get_local_name(resource)),
		   "Incorrect local resource name %s", bionet_resource_get_local_name(resource));
} END_TEST /* test_libutil_resource_get_local_name_2 */


START_TEST (test_libutil_resource_get_local_name_3) {
    fail_if(NULL != bionet_resource_get_local_name(NULL),
	    "Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_local_name_3 */


START_TEST (test_libutil_resource_get_id_0) {
    bionet_hab_t * hab;
    bionet_node_t * node;
    bionet_resource_t * resource;
    char * resname;

    hab = bionet_hab_new("foo", "bar");
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    resource = bionet_resource_new(node, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if(strcmp("resource", bionet_resource_get_id(resource)),
		   "Incorrect local resource name %s", bionet_resource_get_id(resource));
} END_TEST /* test_libutil_resource_get_id_0 */


START_TEST (test_libutil_resource_get_id_1) {
    bionet_node_t * node;
    bionet_resource_t * resource;
    char * resname;

    node = bionet_node_new(NULL, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    resource = bionet_resource_new(node, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if(strcmp("resource", bionet_resource_get_id(resource)),
		   "Incorrect local resource name %s", bionet_resource_get_id(resource));
} END_TEST /* test_libutil_resource_get_id_1 */


START_TEST (test_libutil_resource_get_id_2) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if(strcmp("resource", bionet_resource_get_id(resource)),
		   "Incorrect local resource name %s", bionet_resource_get_id(resource));
} END_TEST /* test_libutil_resource_get_id_2 */


START_TEST (test_libutil_resource_get_id_3) {
    fail_unless(NULL == bionet_resource_get_id(NULL),
		"Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_id_3 */


START_TEST (test_libutil_resource_get_node_0) {
    fail_unless(NULL == bionet_resource_get_node(NULL),
		"Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_node_0 */


START_TEST (test_libutil_resource_get_node_1) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(NULL == bionet_resource_get_node(resource),
		"Resource has no node. Where did one come from?");
} END_TEST /* test_libutil_resource_get_node_1 */


START_TEST (test_libutil_resource_get_node_2) {
    bionet_resource_t * resource;
    bionet_node_t * node;

    node = bionet_node_new(NULL, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    resource = bionet_resource_new(node, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(node == bionet_resource_get_node(resource),
		"Incorrect node fetched from resource.");
} END_TEST /* test_libutil_resource_get_node_2 */


START_TEST (test_libutil_resource_get_hab_0) {
    fail_unless(NULL == bionet_resource_get_hab(NULL),
		"Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_hab_0 */


START_TEST (test_libutil_resource_get_hab_1) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(NULL == bionet_resource_get_hab(resource),
		"Resource has no node. Where did a HAB come from?");
} END_TEST /* test_libutil_resource_get_hab_1 */


START_TEST (test_libutil_resource_get_hab_2) {
    bionet_resource_t * resource;
    bionet_node_t * node;

    node = bionet_node_new(NULL, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    resource = bionet_resource_new(node, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(NULL == bionet_resource_get_hab(resource),
		"Resource has a node with no HAB. Where did a HAB come from?");
} END_TEST /* test_libutil_resource_get_hab_2 */


START_TEST (test_libutil_resource_get_hab_3) {
    bionet_resource_t * resource;
    bionet_node_t * node;
    bionet_hab_t * hab;

    hab = bionet_hab_new("foo", "bar");
    fail_unless(NULL != hab, "Failed to get a new HAB: %m\n");

    node = bionet_node_new(hab, "node");
    fail_unless(NULL != node, "Failed to get a new Node: %m\n");

    resource = bionet_resource_new(node, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(hab == bionet_resource_get_hab(resource),
		"Incorrect HAB fetched.");
} END_TEST /* test_libutil_resource_get_hab_3 */


START_TEST (test_libutil_resource_get_data_type_0) {
    fail_unless(BIONET_RESOURCE_DATA_TYPE_INVALID == bionet_resource_get_data_type(NULL),
		"Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_data_type_0 */


START_TEST (test_libutil_resource_get_data_type_1) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(BIONET_RESOURCE_DATA_TYPE_STRING == bionet_resource_get_data_type(resource),
		"Resource is a STRING. Why is it reporting %d", bionet_resource_get_data_type(resource));
} END_TEST /* test_libutil_resource_get_data_type_1 */


START_TEST (test_libutil_resource_get_flavor_0) {
    fail_unless(BIONET_RESOURCE_FLAVOR_INVALID == bionet_resource_get_flavor(NULL),
		"Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_flavor_0 */


START_TEST (test_libutil_resource_get_flavor_1) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(BIONET_RESOURCE_FLAVOR_PARAMETER == bionet_resource_get_flavor(resource),
		"Resource is a PARAMETER. Why is it reporting %d", bionet_resource_get_flavor(resource));
} END_TEST /* test_libutil_resource_get_flavor_1 */


START_TEST (test_libutil_resource_flavor_to_string_0) {
    fail_if(strcmp("(invalid)", bionet_resource_flavor_to_string(BIONET_RESOURCE_FLAVOR_INVALID)),
	    "Flavor string should be '(invalid)' but is '%s'", bionet_resource_flavor_to_string(BIONET_RESOURCE_FLAVOR_INVALID));
} END_TEST /* test_libutil_resource_flavor_to_string_0 */

START_TEST (test_libutil_resource_flavor_to_string_1) {
    fail_if(strcmp("Sensor", bionet_resource_flavor_to_string(BIONET_RESOURCE_FLAVOR_SENSOR)),
	    "Flavor string should be 'Sensor' but is '%s'", bionet_resource_flavor_to_string(BIONET_RESOURCE_FLAVOR_SENSOR));
} END_TEST /* test_libutil_resource_flavor_to_string_1 */

START_TEST (test_libutil_resource_flavor_to_string_2) {
    fail_if(strcmp("Parameter", bionet_resource_flavor_to_string(BIONET_RESOURCE_FLAVOR_PARAMETER)),
	    "Flavor string should be 'Parameter' but is '%s'", bionet_resource_flavor_to_string(BIONET_RESOURCE_FLAVOR_PARAMETER));
} END_TEST /* test_libutil_resource_flavor_to_string_2 */

START_TEST (test_libutil_resource_flavor_to_string_3) {
    fail_if(strcmp("Actuator", bionet_resource_flavor_to_string(BIONET_RESOURCE_FLAVOR_ACTUATOR)),
	    "Flavor string should be 'Actuator' but is '%s'", bionet_resource_flavor_to_string(BIONET_RESOURCE_FLAVOR_ACTUATOR));
} END_TEST /* test_libutil_resource_flavor_to_string_3 */

START_TEST (test_libutil_resource_flavor_to_string_4) {
    fail_if(strcmp("(invalid)", bionet_resource_flavor_to_string(1000000)),
	    "Flavor string should be '(invalid)' but is '%s'", bionet_resource_flavor_to_string(1000000));
} END_TEST /* test_libutil_resource_flavor_to_string_4 */


START_TEST (test_libutil_resource_flavor_from_string_0) {
    fail_if(BIONET_RESOURCE_FLAVOR_INVALID != bionet_resource_flavor_from_string("foo"),
	    "Flavor should be BIONET_RESOURCE_FLAVOR_INVALID but is %d", bionet_resource_flavor_from_string("foo"));
} END_TEST /* test_libutil_resource_flavor_from_string_0 */

START_TEST (test_libutil_resource_flavor_from_string_1) {
    fail_if(BIONET_RESOURCE_FLAVOR_INVALID != bionet_resource_flavor_from_string(NULL),
	    "Flavor should be BIONET_RESOURCE_FLAVOR_INVALID but is %d", bionet_resource_flavor_from_string(NULL));
} END_TEST /* test_libutil_resource_flavor_from_string_1 */

START_TEST (test_libutil_resource_flavor_from_string_2) {
    fail_if(BIONET_RESOURCE_FLAVOR_SENSOR != bionet_resource_flavor_from_string("Sensor"),
	    "Flavor should be BIONET_RESOURCE_FLAVOR_SENSOR but is %d", bionet_resource_flavor_from_string("Sensor"));
} END_TEST /* test_libutil_resource_flavor_from_string_2 */

START_TEST (test_libutil_resource_flavor_from_string_3) {
    fail_if(BIONET_RESOURCE_FLAVOR_PARAMETER != bionet_resource_flavor_from_string("Parameter"),
	    "Flavor should be BIONET_RESOURCE_FLAVOR_PARAMETER but is %d", bionet_resource_flavor_from_string("Parameter"));
} END_TEST /* test_libutil_resource_flavor_from_string_3 */

START_TEST (test_libutil_resource_flavor_from_string_4) {
    fail_if(BIONET_RESOURCE_FLAVOR_ACTUATOR != bionet_resource_flavor_from_string("Actuator"),
	    "Flavor should be BIONET_RESOURCE_FLAVOR_ACTUATOR but is %d", bionet_resource_flavor_from_string("Actuator"));
} END_TEST /* test_libutil_resource_flavor_from_string_4 */


START_TEST (test_libutil_resource_data_type_to_string_0) {
    fail_if(strcmp("(invalid)", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_INVALID)),
	    "Datatype string should be '(invalid)' but is '%s'", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_INVALID));
} END_TEST /* test_libutil_resource_data_type_to_string_0 */

START_TEST (test_libutil_resource_data_type_to_string_1) {
    fail_if(strcmp("(invalid)", bionet_resource_data_type_to_string(1000000)),
	    "Datatype string should be '(invalid)' but is '%s'", bionet_resource_data_type_to_string(1000000));
} END_TEST /* test_libutil_resource_data_type_to_string_1 */

START_TEST (test_libutil_resource_data_type_to_string_2) {
    fail_if(strcmp("Binary", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_BINARY)),
	    "Datatype string should be 'Binary' but is '%s'", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_BINARY));
} END_TEST /* test_libutil_resource_data_type_to_string_2 */

START_TEST (test_libutil_resource_data_type_to_string_3) {
    fail_if(strcmp("UInt8", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_UINT8)),
	    "Datatype string should be 'Uint8' but is '%s'", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_UINT8));
} END_TEST /* test_libutil_resource_data_type_to_string_3 */

START_TEST (test_libutil_resource_data_type_to_string_4) {
    fail_if(strcmp("Int8", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_INT8)),
	    "Datatype string should be 'Int8' but is '%s'", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_INT8));
} END_TEST /* test_libutil_resource_data_type_to_string_4 */

START_TEST (test_libutil_resource_data_type_to_string_5) {
    fail_if(strcmp("UInt16", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_UINT16)),
	    "Datatype string should be 'Uint16' but is '%s'", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_UINT16));
} END_TEST /* test_libutil_resource_data_type_to_string_5 */

START_TEST (test_libutil_resource_data_type_to_string_6) {
    fail_if(strcmp("Int16", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_INT16)),
	    "Datatype string should be 'Int16' but is '%s'", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_INT16));
} END_TEST /* test_libutil_resource_data_type_to_string_6 */

START_TEST (test_libutil_resource_data_type_to_string_7) {
    fail_if(strcmp("UInt32", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_UINT32)),
	    "Datatype string should be 'Uint32' but is '%s'", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_UINT32));
} END_TEST /* test_libutil_resource_data_type_to_string_7 */

START_TEST (test_libutil_resource_data_type_to_string_8) {
    fail_if(strcmp("Int32", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_INT32)),
	    "Datatype string should be 'Int32' but is '%s'", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_INT32));
} END_TEST /* test_libutil_resource_data_type_to_string_8 */

START_TEST (test_libutil_resource_data_type_to_string_9) {
    fail_if(strcmp("Float", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_FLOAT)),
	    "Datatype string should be 'Float' but is '%s'", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_FLOAT));
} END_TEST /* test_libutil_resource_data_type_to_string_9 */

START_TEST (test_libutil_resource_data_type_to_string_10) {
    fail_if(strcmp("Double", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_DOUBLE)),
	    "Datatype string should be 'Double' but is '%s'", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_DOUBLE));
} END_TEST /* test_libutil_resource_data_type_to_string_10 */

START_TEST (test_libutil_resource_data_type_to_string_11) {
    fail_if(strcmp("String", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_STRING)),
	    "Datatype string should be 'String' but is '%s'", bionet_resource_data_type_to_string(BIONET_RESOURCE_DATA_TYPE_STRING));
} END_TEST /* test_libutil_resource_data_type_to_string_11 */


START_TEST (test_libutil_resource_data_type_from_string_0) {
    fail_if(BIONET_RESOURCE_DATA_TYPE_INVALID != bionet_resource_data_type_from_string("(invalid)"),
	    "Datatype string should be %d but is %d", BIONET_RESOURCE_DATA_TYPE_INVALID, bionet_resource_data_type_from_string("(invalid)"));
} END_TEST /* test_libutil_resource_data_type_from_string_0 */

START_TEST (test_libutil_resource_data_type_from_string_1) {
    fail_if(BIONET_RESOURCE_DATA_TYPE_INVALID != bionet_resource_data_type_from_string(NULL),
	    "Datatype string should be %d but is %d", BIONET_RESOURCE_DATA_TYPE_INVALID, bionet_resource_data_type_from_string(NULL));
} END_TEST /* test_libutil_resource_data_type_from_string_1 */

START_TEST (test_libutil_resource_data_type_from_string_2) {
    fail_if(BIONET_RESOURCE_DATA_TYPE_BINARY != bionet_resource_data_type_from_string("Binary"),
	    "Datatype string should be %d but is %d", BIONET_RESOURCE_DATA_TYPE_BINARY, bionet_resource_data_type_from_string("Binary"));
} END_TEST /* test_libutil_resource_data_type_from_string_2 */

START_TEST (test_libutil_resource_data_type_from_string_3) {
    fail_if(BIONET_RESOURCE_DATA_TYPE_UINT8 != bionet_resource_data_type_from_string("UInt8"),
	    "Datatype string should be %d but is %d", BIONET_RESOURCE_DATA_TYPE_UINT8, bionet_resource_data_type_from_string("UInt8"));
} END_TEST /* test_libutil_resource_data_type_from_string_3 */

START_TEST (test_libutil_resource_data_type_from_string_4) {
    fail_if(BIONET_RESOURCE_DATA_TYPE_INT8 != bionet_resource_data_type_from_string("Int8"),
	    "Datatype string should be %d but is %d", BIONET_RESOURCE_DATA_TYPE_INT8, bionet_resource_data_type_from_string("Int8"));
} END_TEST /* test_libutil_resource_data_type_from_string_4 */

START_TEST (test_libutil_resource_data_type_from_string_5) {
    fail_if(BIONET_RESOURCE_DATA_TYPE_UINT16 != bionet_resource_data_type_from_string("UInt16"),
	    "Datatype string should be %d but is %d", BIONET_RESOURCE_DATA_TYPE_UINT16, bionet_resource_data_type_from_string("UInt16"));
} END_TEST /* test_libutil_resource_data_type_from_string_5 */

START_TEST (test_libutil_resource_data_type_from_string_6) {
    fail_if(BIONET_RESOURCE_DATA_TYPE_INT16 != bionet_resource_data_type_from_string("Int16"),
	    "Datatype string should be %d but is %d", BIONET_RESOURCE_DATA_TYPE_INT16, bionet_resource_data_type_from_string("Int16"));
} END_TEST /* test_libutil_resource_data_type_from_string_6 */

START_TEST (test_libutil_resource_data_type_from_string_7) {
    fail_if(BIONET_RESOURCE_DATA_TYPE_UINT32 != bionet_resource_data_type_from_string("UInt32"),
	    "Datatype string should be %d but is %d", BIONET_RESOURCE_DATA_TYPE_UINT32, bionet_resource_data_type_from_string("UInt32"));
} END_TEST /* test_libutil_resource_data_type_from_string_7 */

START_TEST (test_libutil_resource_data_type_from_string_8) {
    fail_if(BIONET_RESOURCE_DATA_TYPE_INT32 != bionet_resource_data_type_from_string("Int32"),
	    "Datatype string should be %d but is %d", BIONET_RESOURCE_DATA_TYPE_INT32, bionet_resource_data_type_from_string("Int32"));
} END_TEST /* test_libutil_resource_data_type_from_string_8 */

START_TEST (test_libutil_resource_data_type_from_string_9) {
    fail_if(BIONET_RESOURCE_DATA_TYPE_FLOAT != bionet_resource_data_type_from_string("Float"),
	    "Datatype string should be %d but is %d", BIONET_RESOURCE_DATA_TYPE_FLOAT, bionet_resource_data_type_from_string("Float"));
} END_TEST /* test_libutil_resource_data_type_from_string_9 */

START_TEST (test_libutil_resource_data_type_from_string_10) {
    fail_if(BIONET_RESOURCE_DATA_TYPE_DOUBLE != bionet_resource_data_type_from_string("Double"),
	    "Datatype string should be %d but is %d", BIONET_RESOURCE_DATA_TYPE_DOUBLE, bionet_resource_data_type_from_string("Double"));
} END_TEST /* test_libutil_resource_data_type_from_string_10 */

START_TEST (test_libutil_resource_data_type_from_string_11) {
    fail_if(BIONET_RESOURCE_DATA_TYPE_STRING != bionet_resource_data_type_from_string("String"),
	    "Datatype string should be %d but is %d", BIONET_RESOURCE_DATA_TYPE_STRING, bionet_resource_data_type_from_string("String"));
} END_TEST /* test_libutil_resource_data_type_from_string_11 */


START_TEST (test_libutil_resource_set_0) {
    bionet_resource_t * resource;
    bionet_value_t * value;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    value = bionet_value_new_str(resource, "value");
    fail_if (NULL == value, "Failed to create new Bionet Value.");

    fail_if (bionet_resource_set(resource, value, NULL),
	"Failed to set the resource when the timestamp is NULL");
} END_TEST /* test_libutil_resource_set_0 */


START_TEST (test_libutil_resource_set_1) {
    bionet_resource_t * resource;
    bionet_value_t * value;
    struct timeval tv;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    value = bionet_value_new_str(resource, "value");
    fail_if (NULL == value, "Failed to create new Bionet Value.");

    gettimeofday(&tv, NULL);
    fail_if (bionet_resource_set(resource, value, &tv),
	     "Failed to set the resource when there is a defined timestamp");
} END_TEST /* test_libutil_resource_set_1 */


START_TEST (test_libutil_resource_set_2) {
    bionet_resource_t * resource;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless (bionet_resource_set(resource, NULL, NULL),
		 "Failed to detect NULL value passed in.");
} END_TEST /* test_libutil_resource_set_2 */


START_TEST (test_libutil_resource_set_3) {
    bionet_resource_t * resource;
    bionet_value_t * value;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    value = bionet_value_new_str(resource, "value");
    fail_if (NULL == value, "Failed to create new Bionet Value.");

    fail_unless (bionet_resource_set(NULL, value, NULL),
		 "Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_set_3 */


START_TEST (test_libutil_resource_set_binary_0) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_BINARY, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if (bionet_resource_set_binary(resource, 1, NULL),
	"Failed to set the resource when the timestamp is NULL");
} END_TEST /* test_libutil_resource_set_binary_0 */


START_TEST (test_libutil_resource_set_binary_1) {
    bionet_resource_t * resource;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_BINARY, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);
    fail_if (bionet_resource_set_binary(resource, 1, &tv),
	"Failed to set the resource when the timestamp is defined");
} END_TEST /* test_libutil_resource_set_binary_1 */


START_TEST (test_libutil_resource_set_binary_2) {
    fail_unless (bionet_resource_set_binary(NULL, 1, NULL),
		 "Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_set_binary_2 */


START_TEST (test_libutil_resource_set_uint8_0) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT8, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if (bionet_resource_set_uint8(resource, 1, NULL),
	"Failed to set the resource when the timestamp is NULL");
} END_TEST /* test_libutil_resource_set_uint8_0 */


START_TEST (test_libutil_resource_set_uint8_1) {
    bionet_resource_t * resource;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT8, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);
    fail_if (bionet_resource_set_uint8(resource, 1, &tv),
	"Failed to set the resource when the timestamp is defined");
} END_TEST /* test_libutil_resource_set_uint8_1 */


START_TEST (test_libutil_resource_set_uint8_2) {
    fail_unless (bionet_resource_set_uint8(NULL, 1, NULL),
		 "Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_set_uint8_2 */


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

    /* bionet_resource_get_name() */
    tcase_add_test(tc, test_libutil_resource_get_name_0);
    tcase_add_test(tc, test_libutil_resource_get_name_1);
    tcase_add_test(tc, test_libutil_resource_get_name_2);
    tcase_add_test(tc, test_libutil_resource_get_name_3);

    /* bionet_resource_get_local_name() */
    tcase_add_test(tc, test_libutil_resource_get_local_name_0);
    tcase_add_test(tc, test_libutil_resource_get_local_name_1);
    tcase_add_test(tc, test_libutil_resource_get_local_name_2);
    tcase_add_test(tc, test_libutil_resource_get_local_name_3);

    /* bionet_resource_get_id() */
    tcase_add_test(tc, test_libutil_resource_get_id_0);
    tcase_add_test(tc, test_libutil_resource_get_id_1);
    tcase_add_test(tc, test_libutil_resource_get_id_2);
    tcase_add_test(tc, test_libutil_resource_get_id_3);

    /* bionet_resource_get_node() */
    tcase_add_test(tc, test_libutil_resource_get_node_0);
    tcase_add_test(tc, test_libutil_resource_get_node_1);
    tcase_add_test(tc, test_libutil_resource_get_node_2);

    /* bionet_resource_get_hab() */
    tcase_add_test(tc, test_libutil_resource_get_hab_0);
    tcase_add_test(tc, test_libutil_resource_get_hab_1);
    tcase_add_test(tc, test_libutil_resource_get_hab_2);
    tcase_add_test(tc, test_libutil_resource_get_hab_3);

    /* bionet_resource_get_data_type() */
    tcase_add_test(tc, test_libutil_resource_get_data_type_0);
    tcase_add_test(tc, test_libutil_resource_get_data_type_1);

    /* bionet_resource_get_flavor() */
    tcase_add_test(tc, test_libutil_resource_get_flavor_0);
    tcase_add_test(tc, test_libutil_resource_get_flavor_1);

    /* bionet_resource_flavor_to_string() */
    tcase_add_test(tc, test_libutil_resource_flavor_to_string_0);
    tcase_add_test(tc, test_libutil_resource_flavor_to_string_1);
    tcase_add_test(tc, test_libutil_resource_flavor_to_string_2);
    tcase_add_test(tc, test_libutil_resource_flavor_to_string_3);
    tcase_add_test(tc, test_libutil_resource_flavor_to_string_4);

    /* bionet_resource_flavor_from_string() */
    tcase_add_test(tc, test_libutil_resource_flavor_from_string_0);
    tcase_add_test(tc, test_libutil_resource_flavor_from_string_1);
    tcase_add_test(tc, test_libutil_resource_flavor_from_string_2);
    tcase_add_test(tc, test_libutil_resource_flavor_from_string_3);
    tcase_add_test(tc, test_libutil_resource_flavor_from_string_4);

    /* bionet_resource_data_type_to_string() */
    tcase_add_test(tc, test_libutil_resource_data_type_to_string_0);
    tcase_add_test(tc, test_libutil_resource_data_type_to_string_1);
    tcase_add_test(tc, test_libutil_resource_data_type_to_string_2);
    tcase_add_test(tc, test_libutil_resource_data_type_to_string_3);
    tcase_add_test(tc, test_libutil_resource_data_type_to_string_4);
    tcase_add_test(tc, test_libutil_resource_data_type_to_string_5);
    tcase_add_test(tc, test_libutil_resource_data_type_to_string_6);
    tcase_add_test(tc, test_libutil_resource_data_type_to_string_7);
    tcase_add_test(tc, test_libutil_resource_data_type_to_string_8);
    tcase_add_test(tc, test_libutil_resource_data_type_to_string_9);
    tcase_add_test(tc, test_libutil_resource_data_type_to_string_10);
    tcase_add_test(tc, test_libutil_resource_data_type_to_string_11);

    /* bionet_resource_data_type_from_string() */
    tcase_add_test(tc, test_libutil_resource_data_type_from_string_0);
    tcase_add_test(tc, test_libutil_resource_data_type_from_string_1);
    tcase_add_test(tc, test_libutil_resource_data_type_from_string_2);
    tcase_add_test(tc, test_libutil_resource_data_type_from_string_3);
    tcase_add_test(tc, test_libutil_resource_data_type_from_string_4);
    tcase_add_test(tc, test_libutil_resource_data_type_from_string_5);
    tcase_add_test(tc, test_libutil_resource_data_type_from_string_6);
    tcase_add_test(tc, test_libutil_resource_data_type_from_string_7);
    tcase_add_test(tc, test_libutil_resource_data_type_from_string_8);
    tcase_add_test(tc, test_libutil_resource_data_type_from_string_9);
    tcase_add_test(tc, test_libutil_resource_data_type_from_string_10);
    tcase_add_test(tc, test_libutil_resource_data_type_from_string_11);

    /* bionet_resource_set() */
    tcase_add_test(tc, test_libutil_resource_set_0);
    tcase_add_test(tc, test_libutil_resource_set_1);
    tcase_add_test(tc, test_libutil_resource_set_2);
    tcase_add_test(tc, test_libutil_resource_set_3);

    /* bionet_resource_set_binary() */
    tcase_add_test(tc, test_libutil_resource_set_binary_0);
    tcase_add_test(tc, test_libutil_resource_set_binary_1);
    tcase_add_test(tc, test_libutil_resource_set_binary_2);

    /* bionet_resource_set_uint8() */
    tcase_add_test(tc, test_libutil_resource_set_uint8_0);
    tcase_add_test(tc, test_libutil_resource_set_uint8_1);
    tcase_add_test(tc, test_libutil_resource_set_uint8_2);

    /* bionet_resource_set_str() */
    tcase_add_test(tc, test_libutil_resource_set_str_0);

    return;
} /* libutil_resource_tests_suite */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
