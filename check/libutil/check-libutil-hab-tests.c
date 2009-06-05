
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
 * hab_new(type, NULL)
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
 * hab_new(NULL, NULL)
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
} END_TEST /* test_libutil_hab_new_1 */


/*
 * hab_new(type, NULL)
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
} END_TEST /* test_libutil_hab_new_2 */


void libutil_hab_tests_suite(Suite *s)
{
    TCase *tc = tcase_create("bionet_hab_new()");
    suite_add_tcase(s, tc);


    tcase_add_test(tc, test_libutil_hab_new_0);
    tcase_add_test(tc, test_libutil_hab_new_1);
    tcase_add_test(tc, test_libutil_hab_new_2);


    return;
} /* libutil_hab_tests_suite() */
