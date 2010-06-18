
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <check.h>

#include "check-common.h"
#include "bionet-util.h"

#include "check-libutil-hab-tests.h"
#include "check-libutil-node-tests.h"
#include "check-libutil-split-hab-name.h"
#include "check-libutil-split-node-name.h"
#include "check-libutil-split-resource-name.h"
#include "check-libutil-resource-set.h"

int main(int argc, char * argv[])
{
    int i, number_failed = 0;

    suite_t func_unit_test_suite[] = 
        {
            { "Bionet Util HAB Tests", &libutil_hab_tests_suite },
            { "Bionet Util Node Tests", &libutil_node_tests_suite },
            { "Bionet Util Split HAB Name", &libutil_split_hab_name_suite },
            { "Bionet Util Split Node Name", &libutil_split_node_name_suite },
            { "Bionet Util Split Resource Name", &libutil_split_resource_name_suite },
            { "Bionet Util Resource Set", &libutil_resource_set_suite },
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
