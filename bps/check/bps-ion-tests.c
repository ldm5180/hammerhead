#include "check-bps.h"

#include <check.h>
#include <errno.h>
#include <string.h>

#include "bps/bps_socket.h"

/*
 * These tests are for ION specific features
 */

START_TEST (check_bps_setopt_ion_basekey) {
    int r;

    long base_key = 0x100;

    r = bps_setopt(BPST_ION, BPSO_ION_BASEKEY, &base_key, sizeof(base_key));
    fail_unless(0 == r, 
            "bps_setopt() returned error when there was none: %s", strerror(errno));
    
    int fd = bps_socket(0,0,0);
    fail_unless(fd < 0, 
            "bps_socket succeeded when there should be no ION to connect to");

}
END_TEST

START_TEST (check_bps_setopt_ion) {
    int r;

    long base_key = 0;
    r = bps_setopt(BPST_ION+1, BPSO_ION_BASEKEY, &base_key, sizeof(base_key));
    fail_unless(r<0, 
            "bps_setopt() didn't detect bad impl_type");

    r = bps_setopt(BPST_ION, BPSO_ION_BASEKEY+1, &base_key, sizeof(base_key));
    fail_unless(r<0, 
            "bps_setopt() didn't detect bad option");

    r = bps_setopt(BPST_ION, BPSO_ION_BASEKEY, &base_key, sizeof(base_key));
    fail_unless(0 == r, 
            "bps_setopt() error'd when setting basekey to 0");
    
    int fd = bps_socket(0,0,0);
    fail_unless(fd >= 0,
            "bps_socket failed when it should have worked");

    r = bps_close(fd);
    fail_unless(0 == r,
            "bps_socket failed to close: %s", strerror(errno));
}
END_TEST

void bps_ion_tests_init(Suite *s) {
    TCase * tc = tcase_create ("bps-ion");

    tcase_add_test (tc, check_bps_setopt_ion);
    tcase_add_test (tc, check_bps_setopt_ion_basekey);

    suite_add_tcase(s, tc);

}

