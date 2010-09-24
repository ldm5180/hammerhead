
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <check.h>

#include <stdlib.h>
#include <sys/wait.h>

#include "check-bps.h"

Suite * check_bps_suite(void) {
    Suite * s = suite_create ("bundle-protocol-sockets");

    // Add tests
    bsp_api_tests_init(s);
    return s;
}

int main(int argc, char * argv[])
{
    int number_failed = 0;
    int rc;

    Suite *s = check_bps_suite();
    SRunner *sr = srunner_create(s);


    // Start ION
    printf("==============================================================\n");
    rc = system("ionstart -I ion.rc");
    printf("==============================================================\n");
    printf("ionstart => %d\n", WEXITSTATUS(rc));

    // Run all tests
    srunner_run_all(sr, CK_NORMAL);

    printf("==============================================================\n");
    rc = system("ionstop");
    printf("==============================================================\n");

    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
} /* main() */

// vim: ts=8 sw=4 sta
// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// indent-tabs-mode: nil
// End:
