
//
// Copyright (C) 2008-2009, Regents of the University of Colorado.
//


#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include "hardware-abstractor.h"
#include "streamy-hab.h"




bionet_hab_t *this_hab = NULL;

GMainLoop *main_loop = NULL;




int main(int argc, char *argv[]) {
    int bionet_fd;


    g_log_set_default_handler(bionet_glib_log_handler, NULL);


    //
    //  Initialize the HAB & connect to Bionet
    //

    hab_register_callback_stream_subscription(cb_stream_subscription);
    hab_register_callback_stream_unsubscription(cb_stream_unsubscription);
    hab_register_callback_stream_data(cb_stream_data);

    this_hab = bionet_hab_new("Streamy", NULL);

    bionet_fd = hab_connect(this_hab);
    if (bionet_fd < 0) {
        fprintf(stderr, "problem connecting to Bionet, exiting\n");
        return 1;
    }


    //
    //  Make the Nodes to hold the Streams
    //

    make_incoming_node(this_hab);
    make_outgoing_node(this_hab);


    //  
    // set up and start the main loop
    //

    main_loop = g_main_loop_new(NULL, TRUE);

    {
        GIOChannel *ch;
        ch = g_io_channel_unix_new(bionet_fd);
        g_io_add_watch(ch, G_IO_IN, read_from_bionet, NULL);
    }


    g_main_loop_run(main_loop);


    exit(0);
}

