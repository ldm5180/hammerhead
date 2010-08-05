
#include <stdlib.h>
#include "check-bdm.h"

Suite * check_bdm_suite(void) {
    Suite *s = suite_create ("bionet-data-manager");

    // Add test cases
    check_bdm_db_init(s);
    return s;
}

int main (int argc, char * argv[]) {
    int number_failed;
    Suite *s = check_bdm_suite ();
    SRunner *sr = srunner_create (s);
    srunner_run_all (sr, CK_NORMAL);
    number_failed = srunner_ntests_failed (sr);
    srunner_free (sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
