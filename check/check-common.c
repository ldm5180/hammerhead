
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <check.h>

#include "check-common.h"

int run_suite(suite_t suite)
{
    int num_failed;
    Suite *s;
    SRunner *sr;
    
    /* initialize test suite */
    s = suite_create(suite.name);
    suite.fp(s);
    sr = srunner_create(s);
    
    /* run test suite */
    srunner_run_all(sr, CK_NORMAL);
    num_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    printf("\n");
    
    return num_failed;
} /* run_suite */

// vim: ts=8 sw=4 sta
// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// indent-tabs-mode: nil
// End:
