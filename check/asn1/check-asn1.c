
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <check.h>

#include "check-common.h"
#include "check-asn1.h"

int main(int argc, char * argv[])
{
    int i, number_failed = 0;

    suite_t func_unit_test_suite[] = 
        {
            { "Bionet ASN.1 Tests", &bionet_asn1_test_suite },
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
