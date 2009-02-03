
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <check.h>

#include "check-common.h"
#include "bionet-util.h"
#include "check-libutil-hab-tests.h"

int main(int argc, char * argv[])
{
    int i, number_failed = 0;

    suite_t func_unit_test_suite[] = 
        {
            { "Bionet Util HAB Tests", &libutil_hab_tests_suite },
        };

    /* run all the integration test suites */
    for (i = 0; i < SIZEOF(func_unit_test_suite); i++)
    {
        number_failed += run_suite(func_unit_test_suite[i]);
    }

    printf("\n");
    
    return number_failed;
} /* main() */

// vim: ts=8 sw=4 sta
// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// indent-tabs-mode: nil
// End:
