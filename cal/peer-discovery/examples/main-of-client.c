
#include <stdio.h>
#include <unistd.h>

#include <glib.h>

#include "glib-on-cal.h"


void cb_new_hab(bionet_hab_t *new_hab) {
    printf("new hab: '%s'\n", new_hab->name);
}


void cb_lost_hab(bionet_hab_t *lost_hab) {
    printf("lost hab: '%s'\n", lost_hab->name);
}


int main(int argc, char *argv[]) {
    GMainLoop *main_loop = NULL;

    main_loop = g_main_loop_new(NULL, FALSE);

    subscribe_hab_list(cb_new_hab, cb_lost_hab);

    g_main_loop_run(main_loop);

    return 0;
}

