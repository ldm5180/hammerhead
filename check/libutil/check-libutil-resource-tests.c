
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

START_TEST (test_libutil_resource_set_int8_0) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT8, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if (bionet_resource_set_int8(resource, 1, NULL),
	"Failed to set the resource when the timestamp is NULL");
} END_TEST /* test_libutil_resource_set_int8_0 */


START_TEST (test_libutil_resource_set_int8_1) {
    bionet_resource_t * resource;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT8, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);
    fail_if (bionet_resource_set_int8(resource, 1, &tv),
	"Failed to set the resource when the timestamp is defined");
} END_TEST /* test_libutil_resource_set_int8_1 */


START_TEST (test_libutil_resource_set_int8_2) {
    fail_unless (bionet_resource_set_int8(NULL, 1, NULL),
		 "Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_set_int8_2 */


START_TEST (test_libutil_resource_set_uint16_0) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT16, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if (bionet_resource_set_uint16(resource, 1, NULL),
	"Failed to set the resource when the timestamp is NULL");
} END_TEST /* test_libutil_resource_set_uint16_0 */


START_TEST (test_libutil_resource_set_uint16_1) {
    bionet_resource_t * resource;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT16, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);
    fail_if (bionet_resource_set_uint16(resource, 1, &tv),
	"Failed to set the resource when the timestamp is defined");
} END_TEST /* test_libutil_resource_set_uint16_1 */


START_TEST (test_libutil_resource_set_uint16_2) {
    fail_unless (bionet_resource_set_uint16(NULL, 1, NULL),
		 "Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_set_uint16_2 */

START_TEST (test_libutil_resource_set_int16_0) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT16, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if (bionet_resource_set_int16(resource, 1, NULL),
	"Failed to set the resource when the timestamp is NULL");
} END_TEST /* test_libutil_resource_set_int16_0 */


START_TEST (test_libutil_resource_set_int16_1) {
    bionet_resource_t * resource;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT16, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);
    fail_if (bionet_resource_set_int16(resource, 1, &tv),
	"Failed to set the resource when the timestamp is defined");
} END_TEST /* test_libutil_resource_set_int16_1 */


START_TEST (test_libutil_resource_set_int16_2) {
    fail_unless (bionet_resource_set_int16(NULL, 1, NULL),
		 "Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_set_int16_2 */


START_TEST (test_libutil_resource_set_uint32_0) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT32, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if (bionet_resource_set_uint32(resource, 1, NULL),
	"Failed to set the resource when the timestamp is NULL");
} END_TEST /* test_libutil_resource_set_uint32_0 */


START_TEST (test_libutil_resource_set_uint32_1) {
    bionet_resource_t * resource;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT32, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);
    fail_if (bionet_resource_set_uint32(resource, 1, &tv),
	"Failed to set the resource when the timestamp is defined");
} END_TEST /* test_libutil_resource_set_uint32_1 */


START_TEST (test_libutil_resource_set_uint32_2) {
    fail_unless (bionet_resource_set_uint32(NULL, 1, NULL),
		 "Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_set_uint32_2 */

START_TEST (test_libutil_resource_set_int32_0) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT32, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if (bionet_resource_set_int32(resource, 1, NULL),
	"Failed to set the resource when the timestamp is NULL");
} END_TEST /* test_libutil_resource_set_int32_0 */


START_TEST (test_libutil_resource_set_int32_1) {
    bionet_resource_t * resource;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT32, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);
    fail_if (bionet_resource_set_int32(resource, 1, &tv),
	"Failed to set the resource when the timestamp is defined");
} END_TEST /* test_libutil_resource_set_int32_1 */


START_TEST (test_libutil_resource_set_int32_2) {
    fail_unless (bionet_resource_set_int32(NULL, 1, NULL),
		 "Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_set_int32_2 */


START_TEST (test_libutil_resource_set_float_0) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_FLOAT, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if (bionet_resource_set_float(resource, 1.0, NULL),
	"Failed to set the resource when the timestamp is NULL");
} END_TEST /* test_libutil_resource_set_float_0 */


START_TEST (test_libutil_resource_set_float_1) {
    bionet_resource_t * resource;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_FLOAT, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);
    fail_if (bionet_resource_set_float(resource, 1.0, &tv),
	"Failed to set the resource when the timestamp is defined");
} END_TEST /* test_libutil_resource_set_float_1 */


START_TEST (test_libutil_resource_set_float_2) {
    fail_unless (bionet_resource_set_float(NULL, 1.0, NULL),
		 "Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_set_float_2 */


START_TEST (test_libutil_resource_set_double_0) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_DOUBLE, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if (bionet_resource_set_double(resource, 1.0, NULL),
	"Failed to set the resource when the timestamp is NULL");
} END_TEST /* test_libutil_resource_set_double_0 */


START_TEST (test_libutil_resource_set_double_1) {
    bionet_resource_t * resource;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_DOUBLE, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);
    fail_if (bionet_resource_set_double(resource, 1.0, &tv),
	"Failed to set the resource when the timestamp is defined");
} END_TEST /* test_libutil_resource_set_double_1 */


START_TEST (test_libutil_resource_set_double_2) {
    fail_unless (bionet_resource_set_double(NULL, 1.0, NULL),
		 "Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_set_double_2 */


START_TEST (test_libutil_resource_set_string_0) {
    bionet_resource_t * resource;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if (bionet_resource_set_str(resource, "foo", NULL),
	"Failed to set the resource when the timestamp is NULL");
} END_TEST /* test_libutil_resource_set_string_0 */


START_TEST (test_libutil_resource_set_string_1) {
    bionet_resource_t * resource;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);
    fail_if (bionet_resource_set_str(resource, "foo", &tv),
	"Failed to set the resource when the timestamp is defined");
} END_TEST /* test_libutil_resource_set_string_1 */


START_TEST (test_libutil_resource_set_string_2) {
    fail_unless (bionet_resource_set_str(NULL, "foo", NULL),
		 "Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_set_string_2 */


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


START_TEST (test_libutil_resource_get_binary_0) {
    bionet_resource_t * resource;
    int content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_BINARY, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_binary(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_binary(resource, &content, &tv),
	    "Failed to get binary value and timestamp from resource.");

    fail_if(1 != content, "Failed to get the correct content from resource");
    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_binary_0 */


START_TEST (test_libutil_resource_get_binary_1) {
    bionet_resource_t * resource;
    int content;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_BINARY, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_binary(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_binary(resource, &content, NULL),
	    "Failed to get binary value and timestamp from resource when timestamp is NULL");

    fail_if(1 != content, "Failed to get the correct content from resource");
} END_TEST /* test_libutil_resource_get_binary_1 */


START_TEST (test_libutil_resource_get_binary_2) {
    bionet_resource_t * resource;
    int content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_BINARY, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_binary(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_binary(resource, NULL, &tv),
	    "Failed to get binary value and timestamp from resource when content is NULL");

    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_binary_2 */


START_TEST (test_libutil_resource_get_binary_3) {
    bionet_resource_t * resource;
    int content;
    struct timeval tv;

    fail_unless(bionet_resource_get_binary(NULL, &content, &tv),
		"Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_binary_3 */


START_TEST (test_libutil_resource_get_binary_4) {
    bionet_resource_t * resource;
    int content;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_BINARY, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(bionet_resource_get_binary(resource, &content, &tv),
		"Failed to detect the resource has no value.");
} END_TEST /* test_libutil_resource_get_binary_4 */


START_TEST (test_libutil_resource_get_uint8_0) {
    bionet_resource_t * resource;
    uint8_t content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT8, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_uint8(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_uint8(resource, &content, &tv),
	    "Failed to get uint8 value and timestamp from resource.");

    fail_if(1 != content, "Failed to get the correct content from resource");
    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_uint8_0 */


START_TEST (test_libutil_resource_get_uint8_1) {
    bionet_resource_t * resource;
    uint8_t content;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT8, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_uint8(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_uint8(resource, &content, NULL),
	    "Failed to get uint8 value and timestamp from resource when timestamp is NULL");

    fail_if(1 != content, "Failed to get the correct content from resource");
} END_TEST /* test_libutil_resource_get_uint8_1 */


START_TEST (test_libutil_resource_get_uint8_2) {
    bionet_resource_t * resource;
    uint8_t content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT8, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_uint8(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_uint8(resource, NULL, &tv),
	    "Failed to get uint8 value and timestamp from resource when content is NULL");

    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_uint8_2 */


START_TEST (test_libutil_resource_get_uint8_3) {
    bionet_resource_t * resource;
    uint8_t content;
    struct timeval tv;

    fail_unless(bionet_resource_get_uint8(NULL, &content, &tv),
		"Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_uint8_3 */


START_TEST (test_libutil_resource_get_uint8_4) {
    bionet_resource_t * resource;
    uint8_t content;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT8, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(bionet_resource_get_uint8(resource, &content, &tv),
		"Failed to detect the resource has no value.");
} END_TEST /* test_libutil_resource_get_uint8_4 */


START_TEST (test_libutil_resource_get_int8_0) {
    bionet_resource_t * resource;
    int8_t content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT8, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_int8(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_int8(resource, &content, &tv),
	    "Failed to get int8 value and timestamp from resource.");

    fail_if(1 != content, "Failed to get the correct content from resource");
    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_int8_0 */


START_TEST (test_libutil_resource_get_int8_1) {
    bionet_resource_t * resource;
    int8_t content;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT8, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_int8(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_int8(resource, &content, NULL),
	    "Failed to get int8 value and timestamp from resource when timestamp is NULL");

    fail_if(1 != content, "Failed to get the correct content from resource");
} END_TEST /* test_libutil_resource_get_int8_1 */


START_TEST (test_libutil_resource_get_int8_2) {
    bionet_resource_t * resource;
    int8_t content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT8, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_int8(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_int8(resource, NULL, &tv),
	    "Failed to get int8 value and timestamp from resource when content is NULL");

    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_int8_2 */


START_TEST (test_libutil_resource_get_int8_3) {
    bionet_resource_t * resource;
    int8_t content;
    struct timeval tv;

    fail_unless(bionet_resource_get_int8(NULL, &content, &tv),
		"Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_int8_3 */


START_TEST (test_libutil_resource_get_int8_4) {
    bionet_resource_t * resource;
    int8_t content;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT8, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(bionet_resource_get_int8(resource, &content, &tv),
		"Failed to detect the resource has no value.");
} END_TEST /* test_libutil_resource_get_int8_4 */


START_TEST (test_libutil_resource_get_uint16_0) {
    bionet_resource_t * resource;
    uint16_t content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT16, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_uint16(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_uint16(resource, &content, &tv),
	    "Failed to get uint16 value and timestamp from resource.");

    fail_if(1 != content, "Failed to get the correct content from resource");
    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_uint16_0 */


START_TEST (test_libutil_resource_get_uint16_1) {
    bionet_resource_t * resource;
    uint16_t content;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT16, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_uint16(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_uint16(resource, &content, NULL),
	    "Failed to get uint16 value and timestamp from resource when timestamp is NULL");

    fail_if(1 != content, "Failed to get the correct content from resource");
} END_TEST /* test_libutil_resource_get_uint16_1 */


START_TEST (test_libutil_resource_get_uint16_2) {
    bionet_resource_t * resource;
    uint16_t content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT16, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_uint16(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_uint16(resource, NULL, &tv),
	    "Failed to get uint16 value and timestamp from resource when content is NULL");

    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_uint16_2 */


START_TEST (test_libutil_resource_get_uint16_3) {
    bionet_resource_t * resource;
    uint16_t content;
    struct timeval tv;

    fail_unless(bionet_resource_get_uint16(NULL, &content, &tv),
		"Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_uint16_3 */


START_TEST (test_libutil_resource_get_uint16_4) {
    bionet_resource_t * resource;
    uint16_t content;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT16, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(bionet_resource_get_uint16(resource, &content, &tv),
		"Failed to detect the resource has no value.");
} END_TEST /* test_libutil_resource_get_uint16_4 */


START_TEST (test_libutil_resource_get_int16_0) {
    bionet_resource_t * resource;
    int16_t content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT16, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_int16(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_int16(resource, &content, &tv),
	    "Failed to get int16 value and timestamp from resource.");

    fail_if(1 != content, "Failed to get the correct content from resource");
    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_int16_0 */


START_TEST (test_libutil_resource_get_int16_1) {
    bionet_resource_t * resource;
    int16_t content;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT16, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_int16(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_int16(resource, &content, NULL),
	    "Failed to get int16 value and timestamp from resource when timestamp is NULL");

    fail_if(1 != content, "Failed to get the correct content from resource");
} END_TEST /* test_libutil_resource_get_int16_1 */


START_TEST (test_libutil_resource_get_int16_2) {
    bionet_resource_t * resource;
    int16_t content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT16, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_int16(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_int16(resource, NULL, &tv),
	    "Failed to get int16 value and timestamp from resource when content is NULL");

    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_int16_2 */


START_TEST (test_libutil_resource_get_int16_3) {
    bionet_resource_t * resource;
    int16_t content;
    struct timeval tv;

    fail_unless(bionet_resource_get_int16(NULL, &content, &tv),
		"Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_int16_3 */


START_TEST (test_libutil_resource_get_int16_4) {
    bionet_resource_t * resource;
    int16_t content;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT16, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(bionet_resource_get_int16(resource, &content, &tv),
		"Failed to detect the resource has no value.");
} END_TEST /* test_libutil_resource_get_int16_4 */


START_TEST (test_libutil_resource_get_uint32_0) {
    bionet_resource_t * resource;
    uint32_t content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT32, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_uint32(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_uint32(resource, &content, &tv),
	    "Failed to get uint32 value and timestamp from resource.");

    fail_if(1 != content, "Failed to get the correct content from resource");
    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_uint32_0 */


START_TEST (test_libutil_resource_get_uint32_1) {
    bionet_resource_t * resource;
    uint32_t content;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT32, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_uint32(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_uint32(resource, &content, NULL),
	    "Failed to get uint32 value and timestamp from resource when timestamp is NULL");

    fail_if(1 != content, "Failed to get the correct content from resource");
} END_TEST /* test_libutil_resource_get_uint32_1 */


START_TEST (test_libutil_resource_get_uint32_2) {
    bionet_resource_t * resource;
    uint32_t content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT32, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_uint32(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_uint32(resource, NULL, &tv),
	    "Failed to get uint32 value and timestamp from resource when content is NULL");

    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_uint32_2 */


START_TEST (test_libutil_resource_get_uint32_3) {
    bionet_resource_t * resource;
    uint32_t content;
    struct timeval tv;

    fail_unless(bionet_resource_get_uint32(NULL, &content, &tv),
		"Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_uint32_3 */


START_TEST (test_libutil_resource_get_uint32_4) {
    bionet_resource_t * resource;
    uint32_t content;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_UINT32, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(bionet_resource_get_uint32(resource, &content, &tv),
		"Failed to detect the resource has no value.");
} END_TEST /* test_libutil_resource_get_uint32_4 */


START_TEST (test_libutil_resource_get_int32_0) {
    bionet_resource_t * resource;
    int32_t content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT32, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_int32(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_int32(resource, &content, &tv),
	    "Failed to get int32 value and timestamp from resource.");

    fail_if(1 != content, "Failed to get the correct content from resource");
    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_int32_0 */


START_TEST (test_libutil_resource_get_int32_1) {
    bionet_resource_t * resource;
    int32_t content;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT32, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_int32(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_int32(resource, &content, NULL),
	    "Failed to get int32 value and timestamp from resource when timestamp is NULL");

    fail_if(1 != content, "Failed to get the correct content from resource");
} END_TEST /* test_libutil_resource_get_int32_1 */


START_TEST (test_libutil_resource_get_int32_2) {
    bionet_resource_t * resource;
    int32_t content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT32, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_int32(resource, 1, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_int32(resource, NULL, &tv),
	    "Failed to get int32 value and timestamp from resource when content is NULL");

    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_int32_2 */


START_TEST (test_libutil_resource_get_int32_3) {
    bionet_resource_t * resource;
    int32_t content;
    struct timeval tv;

    fail_unless(bionet_resource_get_int32(NULL, &content, &tv),
		"Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_int32_3 */


START_TEST (test_libutil_resource_get_int32_4) {
    bionet_resource_t * resource;
    int32_t content;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_INT32, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(bionet_resource_get_int32(resource, &content, &tv),
		"Failed to detect the resource has no value.");
} END_TEST /* test_libutil_resource_get_int32_4 */


START_TEST (test_libutil_resource_get_float_0) {
    bionet_resource_t * resource;
    float content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_FLOAT, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_float(resource, 1.0, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_float(resource, &content, &tv),
	    "Failed to get float value and timestamp from resource.");

    fail_if(1 != content, "Failed to get the correct content from resource");
    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_float_0 */


START_TEST (test_libutil_resource_get_float_1) {
    bionet_resource_t * resource;
    float content;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_FLOAT, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_float(resource, 1.0, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_float(resource, &content, NULL),
	    "Failed to get float value and timestamp from resource when timestamp is NULL");

    fail_if(1 != content, "Failed to get the correct content from resource");
} END_TEST /* test_libutil_resource_get_float_1 */


START_TEST (test_libutil_resource_get_float_2) {
    bionet_resource_t * resource;
    float content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_FLOAT, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_float(resource, 1.0, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_float(resource, NULL, &tv),
	    "Failed to get float value and timestamp from resource when content is NULL");

    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_float_2 */


START_TEST (test_libutil_resource_get_float_3) {
    bionet_resource_t * resource;
    float content;
    struct timeval tv;

    fail_unless(bionet_resource_get_float(NULL, &content, &tv),
		"Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_float_3 */


START_TEST (test_libutil_resource_get_float_4) {
    bionet_resource_t * resource;
    float content;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_FLOAT, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(bionet_resource_get_float(resource, &content, &tv),
		"Failed to detect the resource has no value.");
} END_TEST /* test_libutil_resource_get_float_4 */


START_TEST (test_libutil_resource_get_double_0) {
    bionet_resource_t * resource;
    double content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_DOUBLE, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_double(resource, 1.0, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_double(resource, &content, &tv),
	    "Failed to get double value and timestamp from resource.");

    fail_if(1 != content, "Failed to get the correct content from resource");
    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_double_0 */


START_TEST (test_libutil_resource_get_double_1) {
    bionet_resource_t * resource;
    double content;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_DOUBLE, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_double(resource, 1.0, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_double(resource, &content, NULL),
	    "Failed to get double value and timestamp from resource when timestamp is NULL");

    fail_if(1 != content, "Failed to get the correct content from resource");
} END_TEST /* test_libutil_resource_get_double_1 */


START_TEST (test_libutil_resource_get_double_2) {
    bionet_resource_t * resource;
    double content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_DOUBLE, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_double(resource, 1.0, &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_double(resource, NULL, &tv),
	    "Failed to get double value and timestamp from resource when content is NULL");

    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_double_2 */


START_TEST (test_libutil_resource_get_double_3) {
    bionet_resource_t * resource;
    double content;
    struct timeval tv;

    fail_unless(bionet_resource_get_double(NULL, &content, &tv),
		"Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_double_3 */


START_TEST (test_libutil_resource_get_double_4) {
    bionet_resource_t * resource;
    double content;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_DOUBLE, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(bionet_resource_get_double(resource, &content, &tv),
		"Failed to detect the resource has no value.");
} END_TEST /* test_libutil_resource_get_double_4 */


START_TEST (test_libutil_resource_get_str_0) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_str(resource, "foo", &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_str(resource, &content, &tv),
	    "Failed to get str value and timestamp from resource.");

    fail_if(strcmp("foo", content), "Failed to get the correct content from resource");
    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_str_0 */


START_TEST (test_libutil_resource_get_str_1) {
    bionet_resource_t * resource;
    char * content;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_str(resource, "foo", &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_str(resource, &content, NULL),
	    "Failed to get str value and timestamp from resource when timestamp is NULL");

    fail_if(strcmp("foo", content), "Failed to get the correct content from resource");
} END_TEST /* test_libutil_resource_get_str_1 */


START_TEST (test_libutil_resource_get_str_2) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;
    struct timeval orig_tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&orig_tv, NULL);
    fail_if (bionet_resource_set_str(resource, "foo", &orig_tv),
	"Failed to set the resource when the timestamp is NULL");

    fail_if(bionet_resource_get_str(resource, NULL, &tv),
	    "Failed to get str value and timestamp from resource when content is NULL");

    fail_if((orig_tv.tv_sec != tv.tv_sec || orig_tv.tv_usec != tv.tv_usec),
	    "Failed to get correct timestamp from resource");
} END_TEST /* test_libutil_resource_get_str_2 */


START_TEST (test_libutil_resource_get_str_3) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;

    fail_unless(bionet_resource_get_str(NULL, &content, &tv),
		"Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_str_3 */


START_TEST (test_libutil_resource_get_str_4) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;

    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(bionet_resource_get_str(resource, &content, &tv),
		"Failed to detect the resource has no value.");
} END_TEST /* test_libutil_resource_get_str_4 */


START_TEST (test_libutil_resource_add_datapoint_0) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;
    bionet_datapoint_t * datapoint;
    bionet_value_t * value;
    bionet_datapoint_t * fetched;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);

    value = bionet_value_new_str(resource, "foo");
    fail_if(NULL == value, "Failed to create new value.");

    datapoint = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, datapoint);
    fail_unless(1 == bionet_resource_get_num_datapoints(resource),
		"Failed to add datapoint.");

    fetched = BIONET_RESOURCE_GET_DATAPOINT(resource);
    fail_unless(datapoint == fetched, "Failed to get the added datapoint.");

    fetched = bionet_resource_get_datapoint_by_index(resource, 0);
    fail_unless(datapoint == fetched, "Failed to get the added datapoint.");
} END_TEST /* test_libutil_resource_add_datapoint_0 */


START_TEST (test_libutil_resource_add_datapoint_1) {
    bionet_datapoint_t * fetched;
    bionet_resource_t * resource;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(0 == bionet_resource_get_num_datapoints(resource),
		"Didn't yet add a datapoint");

    fail_unless(NULL == BIONET_RESOURCE_GET_DATAPOINT(resource), 
	    "No datapoint added, what was possibly fetched?");

    fail_unless(NULL == bionet_resource_get_datapoint_by_index(resource, 0),
	    "No datapoint added, what was possibly fetched?");
} END_TEST /* test_libutil_resource_add_datapoint_1 */


START_TEST (test_libutil_resource_add_datapoint_2) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;
    bionet_datapoint_t * datapoint1;
    bionet_datapoint_t * datapoint2;
    bionet_value_t * value;
    bionet_datapoint_t * fetched;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);

    value = bionet_value_new_str(resource, "foo");
    fail_if(NULL == value, "Failed to create new value.");

    datapoint1 = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint1, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, datapoint1);
    fail_unless(1 == bionet_resource_get_num_datapoints(resource),
		"Failed to add datapoint.");

    datapoint2 = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint2, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, datapoint2);
    fail_unless(2 == bionet_resource_get_num_datapoints(resource),
		"Failed to add datapoint.");

    fetched = BIONET_RESOURCE_GET_DATAPOINT(resource);
    fail_unless(datapoint1 == fetched, "Failed to get the first added datapoint.");

    fetched = bionet_resource_get_datapoint_by_index(resource, 0);
    fail_unless(datapoint1 == fetched, "Failed to get the first added datapoint.");

    fetched = bionet_resource_get_datapoint_by_index(resource, 1);
    fail_unless(datapoint2 == fetched, "Failed to get the second added datapoint.");
} END_TEST /* test_libutil_resource_add_datapoint_2 */


START_TEST (test_libutil_resource_add_datapoint_3) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;
    bionet_datapoint_t * datapoint;
    bionet_value_t * value;
    bionet_datapoint_t * fetched;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);

    value = bionet_value_new_str(resource, "foo");
    fail_if(NULL == value, "Failed to create new value.");

    datapoint = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, datapoint);

    fail_unless(1 == bionet_resource_get_num_datapoints(resource),
		"Failed to add datapoint.");

    fetched = BIONET_RESOURCE_GET_DATAPOINT(resource);
    fail_unless(datapoint == fetched, "Failed to get the added datapoint.");

    fail_if(bionet_resource_get_datapoint_by_index(resource, 1),
	    "Only 1 datapoint added, how was a second one fetched?");
} END_TEST /* test_libutil_resource_add_datapoint_3 */


START_TEST (test_libutil_resource_add_datapoint_4) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;
    bionet_datapoint_t * datapoint;
    bionet_value_t * value;
    bionet_datapoint_t * fetched;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);

    value = bionet_value_new_str(resource, "foo");
    fail_if(NULL == value, "Failed to create new value.");

    datapoint = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(NULL, datapoint);

    fail_unless(0 == bionet_resource_get_num_datapoints(resource),
		"Failed to detect NULL resource.");

    fail_if(BIONET_RESOURCE_GET_DATAPOINT(resource),
	    "Failed to detect there is no datapoint.");

    fail_if(bionet_resource_get_datapoint_by_index(resource, 0),
	    "Failed to detect there is no datapoint.");
} END_TEST /* test_libutil_resource_add_datapoint_4 */


START_TEST (test_libutil_resource_add_datapoint_5) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;
    bionet_datapoint_t * datapoint;
    bionet_value_t * value;
    bionet_datapoint_t * fetched;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);

    value = bionet_value_new_str(resource, "foo");
    fail_if(NULL == value, "Failed to create new value.");

    datapoint = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, NULL);

    fail_unless(0 == bionet_resource_get_num_datapoints(resource),
		"Failed to detect NULL datapoint.");

    fail_if(BIONET_RESOURCE_GET_DATAPOINT(resource),
	    "Failed to detect there is no datapoint.");

    fail_if(bionet_resource_get_datapoint_by_index(resource, 0),
	    "Failed to detect there is no datapoint.");
} END_TEST /* test_libutil_resource_add_datapoint_5 */


START_TEST (test_libutil_resource_get_num_datapoints_0) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;
    bionet_datapoint_t * datapoint;
    bionet_value_t * value;
    bionet_datapoint_t * fetched;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);

    value = bionet_value_new_str(resource, "foo");
    fail_if(NULL == value, "Failed to create new value.");

    datapoint = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, datapoint);
    fail_unless(1 == bionet_resource_get_num_datapoints(resource),
		"Failed to add datapoint.");
} END_TEST /* test_libutil_resource_get_num_datapoints_0 */


START_TEST (test_libutil_resource_get_num_datapoints_1) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;
    bionet_datapoint_t * datapoint;
    bionet_value_t * value;
    bionet_datapoint_t * fetched;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);

    value = bionet_value_new_str(resource, "foo");
    fail_if(NULL == value, "Failed to create new value.");

    datapoint = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint, "Failed to create new datapoint.");

    fail_unless(0 == bionet_resource_get_num_datapoints(resource),
		"Didn't add datapoint yet.");
} END_TEST /* test_libutil_resource_get_num_datapoints_1 */


START_TEST (test_libutil_resource_get_num_datapoints_2) {
    bionet_resource_t * resource;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(0 == bionet_resource_get_num_datapoints(resource),
		"Didn't add datapoint yet.");
} END_TEST /* test_libutil_resource_get_num_datapoints_2 */


START_TEST (test_libutil_resource_get_num_datapoints_3) {
    fail_unless(-1 == bionet_resource_get_num_datapoints(NULL),
		"Failed to detect NULL resource.");
} END_TEST /* test_libutil_resource_get_num_datapoints_3 */


START_TEST (test_libutil_resource_get_datapoint_by_index_0) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;
    bionet_datapoint_t * datapoint;
    bionet_value_t * value;
    bionet_datapoint_t * fetched;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);

    value = bionet_value_new_str(resource, "foo");
    fail_if(NULL == value, "Failed to create new value.");

    datapoint = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, datapoint);
    fail_unless(1 == bionet_resource_get_num_datapoints(resource),
		"Failed to add datapoint.");

    fetched = BIONET_RESOURCE_GET_DATAPOINT(resource);
    fail_unless(datapoint == fetched, "Failed to get the added datapoint.");

    fetched = bionet_resource_get_datapoint_by_index(resource, 0);
    fail_unless(datapoint == fetched, "Failed to get the added datapoint.");
} END_TEST /* test_libutil_resource_get_datapoint_by_index_0 */


START_TEST (test_libutil_resource_get_datapoint_by_index_1) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;
    bionet_datapoint_t * datapoint1;
    bionet_datapoint_t * datapoint2;
    bionet_value_t * value;
    bionet_datapoint_t * fetched;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);

    value = bionet_value_new_str(resource, "foo");
    fail_if(NULL == value, "Failed to create new value.");

    datapoint1 = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint1, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, datapoint1);
    fail_unless(1 == bionet_resource_get_num_datapoints(resource),
		"Failed to add datapoint.");

    datapoint2 = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint2, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, datapoint2);
    fail_unless(2 == bionet_resource_get_num_datapoints(resource),
		"Failed to add datapoint.");

    fetched = BIONET_RESOURCE_GET_DATAPOINT(resource);
    fail_unless(datapoint1 == fetched, "Failed to get the first added datapoint.");

    fetched = bionet_resource_get_datapoint_by_index(resource, 0);
    fail_unless(datapoint1 == fetched, "Failed to get the first added datapoint.");

    fetched = bionet_resource_get_datapoint_by_index(resource, 1);
    fail_unless(datapoint2 == fetched, "Failed to get the second added datapoint.");
} END_TEST /* test_libutil_resource_get_datapoint_by_index_1 */


START_TEST (test_libutil_resource_get_datapoint_by_index_2) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;
    bionet_datapoint_t * datapoint1;
    bionet_datapoint_t * datapoint2;
    bionet_value_t * value;
    bionet_datapoint_t * fetched;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);

    value = bionet_value_new_str(resource, "foo");
    fail_if(NULL == value, "Failed to create new value.");

    datapoint1 = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint1, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, datapoint1);
    fail_unless(1 == bionet_resource_get_num_datapoints(resource),
		"Failed to add datapoint.");

    datapoint2 = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint2, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, datapoint2);
    fail_unless(2 == bionet_resource_get_num_datapoints(resource),
		"Failed to add datapoint.");

    fetched = BIONET_RESOURCE_GET_DATAPOINT(resource);
    fail_unless(datapoint1 == fetched, "Failed to get the first added datapoint.");

    fetched = bionet_resource_get_datapoint_by_index(resource, 0);
    fail_unless(datapoint1 == fetched, "Failed to get the first added datapoint.");

    fetched = bionet_resource_get_datapoint_by_index(resource, 1);
    fail_unless(datapoint2 == fetched, "Failed to get the second added datapoint.");

    fetched = bionet_resource_get_datapoint_by_index(resource, 2);
    fail_unless(NULL == fetched, "Failed to detect out of bounds index.");
} END_TEST /* test_libutil_resource_get_datapoint_by_index_2 */


START_TEST (test_libutil_resource_get_datapoint_by_index_3) {
    bionet_resource_t * resource;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless (NULL == bionet_resource_get_datapoint_by_index(resource, 0),
		 "No datapoints have been added. Where did the datapoint come from?");
} END_TEST /* test_libutil_resource_get_datapoint_by_index_3 */


START_TEST (test_libutil_resource_get_datapoint_by_index_4) {
    fail_unless (NULL == bionet_resource_get_datapoint_by_index(NULL, 0),
		 "Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_get_datapoint_by_index_4 */


START_TEST (test_libutil_resource_remove_datapoint_by_index_0) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;
    bionet_datapoint_t * datapoint;
    bionet_value_t * value;
    bionet_datapoint_t * fetched;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);

    value = bionet_value_new_str(resource, "foo");
    fail_if(NULL == value, "Failed to create new value.");

    datapoint = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, datapoint);
    fail_unless(1 == bionet_resource_get_num_datapoints(resource),
		"Failed to add datapoint.");

    fetched = BIONET_RESOURCE_GET_DATAPOINT(resource);
    fail_unless(datapoint == fetched, "Failed to get the added datapoint.");

    fetched = bionet_resource_get_datapoint_by_index(resource, 0);
    fail_unless(datapoint == fetched, "Failed to get the added datapoint.");

    bionet_resource_remove_datapoint_by_index(resource, 0);
    fail_unless(0 == bionet_resource_get_num_datapoints(resource),
		"Failed to remove datapoint.");
} END_TEST /* test_libutil_resource_remove_datapoint_by_index_0 */


START_TEST (test_libutil_resource_remove_datapoint_by_index_1) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;
    bionet_datapoint_t * datapoint;
    bionet_value_t * value;
    bionet_datapoint_t * fetched;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);

    value = bionet_value_new_str(resource, "foo");
    fail_if(NULL == value, "Failed to create new value.");

    datapoint = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, datapoint);
    fail_unless(1 == bionet_resource_get_num_datapoints(resource),
		"Failed to add datapoint.");

    datapoint = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, datapoint);
    fail_unless(2 == bionet_resource_get_num_datapoints(resource),
		"Failed to add datapoint.");

    bionet_resource_remove_datapoint_by_index(resource, 0);
    fail_unless(1 == bionet_resource_get_num_datapoints(resource),
		"Failed to remove datapoint.");

    fetched = bionet_resource_get_datapoint_by_index(resource, 0);
    fail_unless(datapoint == fetched, "Failed to get the second added datapoint after the first was removed.");
} END_TEST /* test_libutil_resource_remove_datapoint_by_index_1 */


START_TEST (test_libutil_resource_remove_datapoint_by_index_2) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;
    bionet_datapoint_t * datapoint1;
    bionet_datapoint_t * datapoint2;
    bionet_value_t * value;
    bionet_datapoint_t * fetched;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);

    value = bionet_value_new_str(resource, "foo");
    fail_if(NULL == value, "Failed to create new value.");

    datapoint1 = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint1, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, datapoint1);
    fail_unless(1 == bionet_resource_get_num_datapoints(resource),
		"Failed to add datapoint.");

    datapoint2 = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint2, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, datapoint2);
    fail_unless(2 == bionet_resource_get_num_datapoints(resource),
		"Failed to add datapoint.");

    bionet_resource_remove_datapoint_by_index(resource, 1);
    fail_unless(1 == bionet_resource_get_num_datapoints(resource),
		"Failed to remove datapoint.");

    fetched = bionet_resource_get_datapoint_by_index(resource, 0);
    fail_unless(datapoint1 == fetched, "Failed to get the first added datapoint after the second was removed.");
} END_TEST /* test_libutil_resource_remove_datapoint_by_index_2 */


START_TEST (test_libutil_resource_remove_datapoint_by_index_3) {
    bionet_resource_t * resource;
    char * content;
    struct timeval tv;
    bionet_datapoint_t * datapoint;
    bionet_value_t * value;
    bionet_datapoint_t * fetched;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    gettimeofday(&tv, NULL);

    value = bionet_value_new_str(resource, "foo");
    fail_if(NULL == value, "Failed to create new value.");

    datapoint = bionet_datapoint_new(resource, value, &tv);
    fail_if(NULL == datapoint, "Failed to create new datapoint.");

    bionet_resource_add_datapoint(resource, datapoint);
    fail_unless(1 == bionet_resource_get_num_datapoints(resource),
		"Failed to add datapoint.");

    fetched = BIONET_RESOURCE_GET_DATAPOINT(resource);
    fail_unless(datapoint == fetched, "Failed to get the added datapoint.");

    bionet_resource_remove_datapoint_by_index(resource, 0);
    fail_unless(0 == bionet_resource_get_num_datapoints(resource),
		"Failed to remove datapoint.");

    fetched = bionet_resource_get_datapoint_by_index(resource, 0);
    fail_unless(NULL == fetched, "Where did another datapoint come from?");
} END_TEST /* test_libutil_resource_remove_datapoint_by_index_3 */


START_TEST (test_libutil_resource_remove_datapoint_by_index_4) {
    bionet_datapoint_t * fetched;

    fetched = bionet_resource_get_datapoint_by_index(NULL, 0);
    fail_unless(NULL == fetched, "Failed to detect NULL resource.");
} END_TEST /* test_libutil_resource_remove_datapoint_by_index_4 */


START_TEST (test_libutil_resource_matches_id_0) {
    bionet_resource_t * resource;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(bionet_resource_matches_id(resource, "resource"),
		"'resource' does not match resource ID %s", bionet_resource_get_id(resource));
} END_TEST /* test_libutil_resource_matches_id_0 */


START_TEST (test_libutil_resource_matches_id_1) {
    bionet_resource_t * resource;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_unless(bionet_resource_matches_id(resource, "*"),
		"'*' does not match resource ID %s", bionet_resource_get_id(resource));
} END_TEST /* test_libutil_resource_matches_id_1 */


START_TEST (test_libutil_resource_matches_id_2) {
    bionet_resource_t * resource;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resource");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if(bionet_resource_matches_id(resource, NULL),
	    "Failed to detect NULL ID passed in.");
} END_TEST /* test_libutil_resource_matches_id_2 */


START_TEST (test_libutil_resource_matches_id_3) {
    fail_if(bionet_resource_matches_id(NULL, "resource"),
	    "Failed to detect NULL resource passed in.");
} END_TEST /* test_libutil_resource_matches_id_3 */


START_TEST (test_libutil_resource_matches_id_4) {
    bionet_resource_t * resource;
    
    resource = bionet_resource_new(NULL, 
				   BIONET_RESOURCE_DATA_TYPE_STRING, 
				   BIONET_RESOURCE_FLAVOR_PARAMETER, 
				   "resourcenot");
    fail_if(resource == NULL, "failed to create a perfectly normal resource\n");

    fail_if(bionet_resource_matches_id(resource, "resource"),
	    "'resource' does not match resource ID %s", bionet_resource_get_id(resource));
} END_TEST /* test_libutil_resource_matches_id_4 */


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

    /* bionet_resource_set_int8() */
    tcase_add_test(tc, test_libutil_resource_set_int8_0);
    tcase_add_test(tc, test_libutil_resource_set_int8_1);
    tcase_add_test(tc, test_libutil_resource_set_int8_2);

    /* bionet_resource_set_uint16() */
    tcase_add_test(tc, test_libutil_resource_set_uint16_0);
    tcase_add_test(tc, test_libutil_resource_set_uint16_1);
    tcase_add_test(tc, test_libutil_resource_set_uint16_2);

    /* bionet_resource_set_int16() */
    tcase_add_test(tc, test_libutil_resource_set_int16_0);
    tcase_add_test(tc, test_libutil_resource_set_int16_1);
    tcase_add_test(tc, test_libutil_resource_set_int16_2);

    /* bionet_resource_set_uint32() */
    tcase_add_test(tc, test_libutil_resource_set_uint32_0);
    tcase_add_test(tc, test_libutil_resource_set_uint32_1);
    tcase_add_test(tc, test_libutil_resource_set_uint32_2);

    /* bionet_resource_set_int32() */
    tcase_add_test(tc, test_libutil_resource_set_int32_0);
    tcase_add_test(tc, test_libutil_resource_set_int32_1);
    tcase_add_test(tc, test_libutil_resource_set_int32_2);

    /* bionet_resource_set_float() */
    tcase_add_test(tc, test_libutil_resource_set_float_0);
    tcase_add_test(tc, test_libutil_resource_set_float_1);
    tcase_add_test(tc, test_libutil_resource_set_float_2);

    /* bionet_resource_set_double() */
    tcase_add_test(tc, test_libutil_resource_set_double_0);
    tcase_add_test(tc, test_libutil_resource_set_double_1);
    tcase_add_test(tc, test_libutil_resource_set_double_2);

    /* bionet_resource_set_string() */
    tcase_add_test(tc, test_libutil_resource_set_string_0);
    tcase_add_test(tc, test_libutil_resource_set_string_1);
    tcase_add_test(tc, test_libutil_resource_set_string_2);

    /* bionet_resource_set_str() */
    tcase_add_test(tc, test_libutil_resource_set_str_0);

    /* bionet_resource_get_binary() */
    tcase_add_test(tc, test_libutil_resource_get_binary_0);
    tcase_add_test(tc, test_libutil_resource_get_binary_1);
    tcase_add_test(tc, test_libutil_resource_get_binary_2);
    tcase_add_test(tc, test_libutil_resource_get_binary_3);
    tcase_add_test(tc, test_libutil_resource_get_binary_4);

    /* bionet_resource_get_uint8() */
    tcase_add_test(tc, test_libutil_resource_get_uint8_0);
    tcase_add_test(tc, test_libutil_resource_get_uint8_1);
    tcase_add_test(tc, test_libutil_resource_get_uint8_2);
    tcase_add_test(tc, test_libutil_resource_get_uint8_3);
    tcase_add_test(tc, test_libutil_resource_get_uint8_4);

    /* bionet_resource_get_int8() */
    tcase_add_test(tc, test_libutil_resource_get_int8_0);
    tcase_add_test(tc, test_libutil_resource_get_int8_1);
    tcase_add_test(tc, test_libutil_resource_get_int8_2);
    tcase_add_test(tc, test_libutil_resource_get_int8_3);
    tcase_add_test(tc, test_libutil_resource_get_int8_4);

    /* bionet_resource_get_uint16() */
    tcase_add_test(tc, test_libutil_resource_get_uint16_0);
    tcase_add_test(tc, test_libutil_resource_get_uint16_1);
    tcase_add_test(tc, test_libutil_resource_get_uint16_2);
    tcase_add_test(tc, test_libutil_resource_get_uint16_3);
    tcase_add_test(tc, test_libutil_resource_get_uint16_4);

    /* bionet_resource_get_int16() */
    tcase_add_test(tc, test_libutil_resource_get_int16_0);
    tcase_add_test(tc, test_libutil_resource_get_int16_1);
    tcase_add_test(tc, test_libutil_resource_get_int16_2);
    tcase_add_test(tc, test_libutil_resource_get_int16_3);
    tcase_add_test(tc, test_libutil_resource_get_int16_4);

    /* bionet_resource_get_uint32() */
    tcase_add_test(tc, test_libutil_resource_get_uint32_0);
    tcase_add_test(tc, test_libutil_resource_get_uint32_1);
    tcase_add_test(tc, test_libutil_resource_get_uint32_2);
    tcase_add_test(tc, test_libutil_resource_get_uint32_3);
    tcase_add_test(tc, test_libutil_resource_get_uint32_4);

    /* bionet_resource_get_int32() */
    tcase_add_test(tc, test_libutil_resource_get_int32_0);
    tcase_add_test(tc, test_libutil_resource_get_int32_1);
    tcase_add_test(tc, test_libutil_resource_get_int32_2);
    tcase_add_test(tc, test_libutil_resource_get_int32_3);
    tcase_add_test(tc, test_libutil_resource_get_int32_4);

    /* bionet_resource_get_float() */
    tcase_add_test(tc, test_libutil_resource_get_float_0);
    tcase_add_test(tc, test_libutil_resource_get_float_1);
    tcase_add_test(tc, test_libutil_resource_get_float_2);
    tcase_add_test(tc, test_libutil_resource_get_float_3);
    tcase_add_test(tc, test_libutil_resource_get_float_4);

    /* bionet_resource_get_double() */
    tcase_add_test(tc, test_libutil_resource_get_double_0);
    tcase_add_test(tc, test_libutil_resource_get_double_1);
    tcase_add_test(tc, test_libutil_resource_get_double_2);
    tcase_add_test(tc, test_libutil_resource_get_double_3);
    tcase_add_test(tc, test_libutil_resource_get_double_4);

    /* bionet_resource_get_str() */
    tcase_add_test(tc, test_libutil_resource_get_str_0);
    tcase_add_test(tc, test_libutil_resource_get_str_1);
    tcase_add_test(tc, test_libutil_resource_get_str_2);
    tcase_add_test(tc, test_libutil_resource_get_str_3);
    tcase_add_test(tc, test_libutil_resource_get_str_4);

    /* bionet_resource_add_datapoint() */
    tcase_add_test(tc, test_libutil_resource_add_datapoint_0);
    tcase_add_test(tc, test_libutil_resource_add_datapoint_1);
    tcase_add_test(tc, test_libutil_resource_add_datapoint_2);
    tcase_add_test(tc, test_libutil_resource_add_datapoint_3);
    tcase_add_test(tc, test_libutil_resource_add_datapoint_4);
    tcase_add_test(tc, test_libutil_resource_add_datapoint_5);

    /* bionet_resource_get_num_datapoints() */
    tcase_add_test(tc, test_libutil_resource_get_num_datapoints_0);
    tcase_add_test(tc, test_libutil_resource_get_num_datapoints_1);
    tcase_add_test(tc, test_libutil_resource_get_num_datapoints_2);
    tcase_add_test(tc, test_libutil_resource_get_num_datapoints_3);

    /* bionet_resource_get_datapoint_by_index() */
    tcase_add_test(tc, test_libutil_resource_get_datapoint_by_index_0);
    tcase_add_test(tc, test_libutil_resource_get_datapoint_by_index_1);
    tcase_add_test(tc, test_libutil_resource_get_datapoint_by_index_2);
    tcase_add_test(tc, test_libutil_resource_get_datapoint_by_index_3);
    tcase_add_test(tc, test_libutil_resource_get_datapoint_by_index_4);

    /* bionet_resource_remove_datapoint_by_index() */
    tcase_add_test(tc, test_libutil_resource_remove_datapoint_by_index_0);
    tcase_add_test(tc, test_libutil_resource_remove_datapoint_by_index_1);
    tcase_add_test(tc, test_libutil_resource_remove_datapoint_by_index_2);
    tcase_add_test(tc, test_libutil_resource_remove_datapoint_by_index_3);
    tcase_add_test(tc, test_libutil_resource_remove_datapoint_by_index_4);

    /* bionet_resource_matches_id() */
    tcase_add_test(tc, test_libutil_resource_matches_id_0);
    tcase_add_test(tc, test_libutil_resource_matches_id_1);
    tcase_add_test(tc, test_libutil_resource_matches_id_2);
    tcase_add_test(tc, test_libutil_resource_matches_id_3);
    tcase_add_test(tc, test_libutil_resource_matches_id_4);


    return;
} /* libutil_resource_tests_suite */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
