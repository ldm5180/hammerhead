
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


void libutil_hab_tests_suite(Suite *s)
{
    TCase *tc = tcase_create("bionet_hab_new()");
    suite_add_tcase(s, tc);


    tcase_add_test(tc, test_libutil_hab_new_0);


    return;
} /* libutil_hab_tests_suite() */
