
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <check.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "check-bps.h"

Suite * check_bps_suite(void) {
    Suite * s = suite_create ("bundle-protocol-sockets");

    // Add tests
    bps_api_tests_init(s);
    bps_ion_tests_init(s);
    return s;
}

int main(int argc, char * argv[])
{
    int number_failed = 0;
    int rc;

    // Start ION
    printf("==============================================================\n");
    printf("Starting ION for ALL checks...\n");
    rc = system("ionstart -I ion.rc");
    printf("ionstart => %d\n", WEXITSTATUS(rc));
    printf("==============================================================\n");


    Suite *s = check_bps_suite();
    SRunner *sr = srunner_create(s);

    // Run all tests
    srunner_run_all(sr, CK_NORMAL);

    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    alarm(0); // This should have happened in srunner_free :(

    printf("==============================================================\n");
    printf("Stopping ION\n");
    rc = system("ionstop");
    printf("==============================================================\n");
    printf("\n");

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
} /* main() */

// vim: ts=8 sw=4 sta
// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// indent-tabs-mode: nil
// End:
