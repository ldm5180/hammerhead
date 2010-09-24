
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
#include "protected.h"
#include "check-libutil-epsilon-tests.h"


START_TEST (test_libutil_epsilon_new_binary_0) {
    bionet_epsilon_t * epsilon;
    int val = 0;
    int content;

    epsilon = bionet_epsilon_new_binary(val);
    fail_unless(NULL != epsilon, "Failed to get a new epsilon.\n");

    fail_if(bionet_epsilon_get_binary(epsilon, &content),
	    "Failed to get value of binary.\n");
    
    fail_if(val != content, "Failed to get value set from binary epsilon.\n");
} END_TEST /* test_libutil_epsilon_new_binary_0 */


START_TEST (test_libutil_epsilon_new_binary_1) {
    bionet_epsilon_t * epsilon;
    int val = 1;
    int content;

    epsilon = bionet_epsilon_new_binary(val);
    fail_unless(NULL != epsilon, "Failed to get a new epsilon.");

    fail_if(bionet_epsilon_get_binary(epsilon, &content),
	    "Failed to get value of binary.\n");
    
    fail_if(val != content, "Failed to get value set from binary epsilon.\n");
} END_TEST /* test_libutil_epsilon_new_binary_1 */


START_TEST (test_libutil_epsilon_new_binary_2) {
    bionet_epsilon_t * epsilon;
    int val = 2;
    int content;

    epsilon = bionet_epsilon_new_binary(val);
    fail_unless(NULL != epsilon, "Failed to get a new epsilon.");

    fail_if(bionet_epsilon_get_binary(epsilon, &content),
	    "Failed to get value of binary.\n");
    
    fail_if(1 != content, "Failed to get value set from binary epsilon.\n");
} END_TEST /* test_libutil_epsilon_new_binary_2 */


START_TEST (test_libutil_epsilon_new_uint8_0) {
    bionet_epsilon_t * epsilon;
    uint8_t val = 1;
    uint8_t content;

    epsilon = bionet_epsilon_new_uint8(val);
    fail_unless(NULL != epsilon, "Failed to get a new epsilon.\n");

    fail_if(bionet_epsilon_get_uint8(epsilon, &content),
	    "Failed to get value.\n");
    
    fail_if(val != content, "Failed to get value set from epsilon.\n");
} END_TEST /* test_libutil_epsilon_new_uint8_0 */


START_TEST (test_libutil_epsilon_new_int8_0) {
    bionet_epsilon_t * epsilon;
    int8_t val = 1;
    int8_t content;

    epsilon = bionet_epsilon_new_int8(val);
    fail_unless(NULL != epsilon, "Failed to get a new epsilon.\n");

    fail_if(bionet_epsilon_get_int8(epsilon, &content),
	    "Failed to get value.\n");
    
    fail_if(val != content, "Failed to get value set from epsilon.\n");
} END_TEST /* test_libutil_epsilon_new_int8_0 */


START_TEST (test_libutil_epsilon_new_uint16_0) {
    bionet_epsilon_t * epsilon;
    uint16_t val = 1;
    uint16_t content;

    epsilon = bionet_epsilon_new_uint16(val);
    fail_unless(NULL != epsilon, "Failed to get a new epsilon.\n");

    fail_if(bionet_epsilon_get_uint16(epsilon, &content),
	    "Failed to get value.\n");
    
    fail_if(val != content, "Failed to get value set from epsilon.\n");
} END_TEST /* test_libutil_epsilon_new_uint16_0 */


START_TEST (test_libutil_epsilon_new_int16_0) {
    bionet_epsilon_t * epsilon;
    int16_t val = 1;
    int16_t content;

    epsilon = bionet_epsilon_new_int16(val);
    fail_unless(NULL != epsilon, "Failed to get a new epsilon.\n");

    fail_if(bionet_epsilon_get_int16(epsilon, &content),
	    "Failed to get value.\n");
    
    fail_if(val != content, "Failed to get value set from epsilon.\n");
} END_TEST /* test_libutil_epsilon_new_int16_0 */


START_TEST (test_libutil_epsilon_new_uint32_0) {
    bionet_epsilon_t * epsilon;
    uint32_t val = 1;
    uint32_t content;

    epsilon = bionet_epsilon_new_uint32(val);
    fail_unless(NULL != epsilon, "Failed to get a new epsilon.\n");

    fail_if(bionet_epsilon_get_uint32(epsilon, &content),
	    "Failed to get value.\n");
    
    fail_if(val != content, "Failed to get value set from epsilon.\n");
} END_TEST /* test_libutil_epsilon_new_uint32_0 */


START_TEST (test_libutil_epsilon_new_int32_0) {
    bionet_epsilon_t * epsilon;
    int32_t val = 1;
    int32_t content;

    epsilon = bionet_epsilon_new_int32(val);
    fail_unless(NULL != epsilon, "Failed to get a new epsilon.\n");

    fail_if(bionet_epsilon_get_int32(epsilon, &content),
	    "Failed to get value.\n");
    
    fail_if(val != content, "Failed to get value set from epsilon.\n");
} END_TEST /* test_libutil_epsilon_new_int32_0 */


START_TEST (test_libutil_epsilon_new_float_0) {
    bionet_epsilon_t * epsilon;
    float val = 1.0;
    float content;

    epsilon = bionet_epsilon_new_float(val);
    fail_unless(NULL != epsilon, "Failed to get a new epsilon.\n");

    fail_if(bionet_epsilon_get_float(epsilon, &content),
	    "Failed to get value.\n");
    
    fail_if(val != content, "Failed to get value set from epsilon.\n");
} END_TEST /* test_libutil_epsilon_new_float_0 */


START_TEST (test_libutil_epsilon_new_double_0) {
    bionet_epsilon_t * epsilon;
    double val = 1.0;
    double content;

    epsilon = bionet_epsilon_new_double(val);
    fail_unless(NULL != epsilon, "Failed to get a new epsilon.\n");

    fail_if(bionet_epsilon_get_double(epsilon, &content),
	    "Failed to get value.\n");
    
    fail_if(val != content, "Failed to get value set from epsilon.\n");
} END_TEST /* test_libutil_epsilon_new_double_0 */


START_TEST (test_libutil_epsilon_new_str_0) {
    bionet_epsilon_t * epsilon;
    int val = 1;
    int content;

    epsilon = bionet_epsilon_new_str(val);
    fail_unless(NULL != epsilon, "Failed to get a new epsilon.\n");

    fail_if(bionet_epsilon_get_str(epsilon, &content),
	    "Failed to get value.\n");
    
    fail_if(val != content, "Failed to get value set from epsilon. %d != %d\n", val, content);
} END_TEST /* test_libutil_epsilon_new_str_0 */


START_TEST (test_libutil_epsilon_free_0) {
    bionet_epsilon_t * epsilon;
    int val = 1;

    epsilon = bionet_epsilon_new_str(val);
    fail_unless(NULL != epsilon, "Failed to get a new epsilon.\n");

    bionet_epsilon_free(epsilon);
} END_TEST /* test_libutil_epsilon_free_0 */


START_TEST (test_libutil_epsilon_free_1) {
    bionet_epsilon_free(NULL);
} END_TEST /* test_libutil_epsilon_free_1 */


START_TEST (test_libutil_epsilon_to_str_0) {
    bionet_epsilon_t * epsilon;
    int val = 123;
    char * content_str;

    epsilon = bionet_epsilon_new_uint8(val);
    fail_unless(NULL != epsilon, "Failed to get a new epsilon.\n");

    content_str = bionet_epsilon_to_str(epsilon, BIONET_RESOURCE_DATA_TYPE_UINT8);
    fail_unless(0 == strcmp("123", content_str),
		"Failed to get the correct content string. %s != %s\n", "123", content_str);
} END_TEST /* test_libutil_epsilon_to_str_0 */


START_TEST (test_libutil_epsilon_to_str_1) {
    bionet_epsilon_t * epsilon;
    int val = 123;
    char * content_str;

    epsilon = bionet_epsilon_new_uint8(val);
    fail_unless(NULL != epsilon, "Failed to get a new epsilon.\n");

    content_str = bionet_epsilon_to_str(NULL, BIONET_RESOURCE_DATA_TYPE_UINT8);
    fail_unless(NULL == content_str,
		"Failed to detect NULL epsilon passed in.");
} END_TEST /* test_libutil_epsilon_to_str_1 */


void libutil_epsilon_tests_suite(Suite *s)
{
    TCase *tc = tcase_create("Bionet Epsilon");
    suite_add_tcase(s, tc);

    /* bionet_epsilon_new_binary() */
    tcase_add_test(tc, test_libutil_epsilon_new_binary_0);
    tcase_add_test(tc, test_libutil_epsilon_new_binary_1);
    tcase_add_test(tc, test_libutil_epsilon_new_binary_2);

    /* bionet_epsilon_new_uint8() */
    tcase_add_test(tc, test_libutil_epsilon_new_uint8_0);

    /* bionet_epsilon_new_int8() */
    tcase_add_test(tc, test_libutil_epsilon_new_int8_0);

    /* bionet_epsilon_new_uint16() */
    tcase_add_test(tc, test_libutil_epsilon_new_uint16_0);

    /* bionet_epsilon_new_int16() */
    tcase_add_test(tc, test_libutil_epsilon_new_int16_0);

    /* bionet_epsilon_new_uint32() */
    tcase_add_test(tc, test_libutil_epsilon_new_uint32_0);

    /* bionet_epsilon_new_int32() */
    tcase_add_test(tc, test_libutil_epsilon_new_int32_0);

    /* bionet_epsilon_new_float() */
    tcase_add_test(tc, test_libutil_epsilon_new_float_0);

    /* bionet_epsilon_new_double() */
    tcase_add_test(tc, test_libutil_epsilon_new_double_0);

    /* bionet_epsilon_new_str() */
    tcase_add_test(tc, test_libutil_epsilon_new_str_0);

    /* bionet_epsilon_free() */
    tcase_add_test(tc, test_libutil_epsilon_free_0);
    tcase_add_test(tc, test_libutil_epsilon_free_1);

    /* bionet_epsilon_to_str() */
    tcase_add_test(tc, test_libutil_epsilon_to_str_0);
    tcase_add_test(tc, test_libutil_epsilon_to_str_1);
    

    return;
} /* libutil_hab_tests_suite() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
