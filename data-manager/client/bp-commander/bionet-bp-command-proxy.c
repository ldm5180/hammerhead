
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#include <sys/times.h>

#include <getopt.h>

#include <glib.h>

#include <bp.h>

#include "bionet.h"




static void usage(void) {
    printf(
        "usage: bionet-bp-command-proxy [OPTIONS] EID\n"
        "\n"
        "OPTIONS:\n"
        "    --help, -h             Prints this help.\n"
        "\n"
        "EID:  The endpoint identifier to receive bundles on.\n"
        "\n"
        "If all initialization goes well, the commanding proxy backgrounds itself.\n"
        "All logging output goes to the syslog.\n"
    );
}

static int running;
static BpSAP    sap;

static void handleQuit() {
    running = 0;
    bp_interrupt(sap);
}




void handle_bundle(Sdr *sdr, BpDelivery *dlv) {
    char *resource_name;
    int resource_name_strlen;
    int resource_name_buflen;

    char *value;
    int value_strlen;
    int value_buflen;

    int r;

    int payload_length;
    ZcoReader reader;

    syslog(LOG_INFO, "parsing Bionet command bundle...");

    payload_length = zco_source_data_length(*sdr, dlv->adu);

    sdr_begin_xn(*sdr);
    zco_start_receiving(*sdr, dlv->adu, &reader);


    //
    // read the resource name
    //

    resource_name_strlen = 0;
    resource_name_buflen = 2;

    resource_name = (char *)malloc(resource_name_buflen);
    if (resource_name == NULL) {
        syslog(LOG_ERR, "out of memory!");
        zco_stop_receiving(*sdr, &reader);
        sdr_end_xn(*sdr);
        return;
    }

    do {
        if ((resource_name_strlen + 1) > resource_name_buflen) {
            // need to grow the resource name buffer
            char *new_resource_name;
            resource_name_buflen *= 2;
            new_resource_name = (char *)realloc(resource_name, resource_name_buflen);
            if (new_resource_name == NULL) {
                syslog(LOG_ERR, "out of memory!");
                free(resource_name);
                zco_stop_receiving(*sdr, &reader);
                sdr_end_xn(*sdr);
                return;
            }
            resource_name = new_resource_name;
        }

        r = zco_receive_source(*sdr, &reader, 1, (void*)&resource_name[resource_name_strlen]);
        if (r < 0) {
            syslog(LOG_ERR, "error from zco_receive_source(), dropping bundle");
            free(resource_name);
            zco_stop_receiving(*sdr, &reader);
            sdr_end_xn(*sdr);
            return;
        }
        if (r != 1) {
            syslog(LOG_ERR, "failed to read next byte of resource name, dropping bundle!\n");
            free(resource_name);
            zco_stop_receiving(*sdr, &reader);
            sdr_end_xn(*sdr);
            return;
        }
        resource_name_strlen ++;
    } while(resource_name[resource_name_strlen-1] != (char)0);


    //
    // read the value
    //

    value_strlen = 0;
    value_buflen = 2;

    value = (char *)malloc(value_buflen);
    if (value == NULL) {
        syslog(LOG_ERR, "out of memory!");
        free(resource_name);
        zco_stop_receiving(*sdr, &reader);
        sdr_end_xn(*sdr);
        return;
    }

    do {
        if ((value_strlen + 1) > value_buflen) {
            // need to grow the value string buffer
            char *new_value;
            value_buflen *= 2;
            new_value = (char *)realloc(value, value_buflen);
            if (new_value == NULL) {
                syslog(LOG_ERR, "out of memory!");
                free(resource_name);
                free(value);
                zco_stop_receiving(*sdr, &reader);
                sdr_end_xn(*sdr);
                return;
            }
            value = new_value;
        }

        r = zco_receive_source(*sdr, &reader, 1, (void*)&value[value_strlen]);
        if (r < 0) {
            syslog(LOG_ERR, "error from zco_receive_source(), dropping bundle");
            free(resource_name);
            free(value);
            zco_stop_receiving(*sdr, &reader);
            sdr_end_xn(*sdr);
            return;
        }
        if (r != 1) {
            syslog(LOG_ERR, "failed to read next byte of value, dropping bundle!\n");
            free(resource_name);
            free(value);
            zco_stop_receiving(*sdr, &reader);
            sdr_end_xn(*sdr);
            return;
        }
        value_strlen ++;
    } while(value[value_strlen-1] != (char)0);

    // FIXME: verify strings are sane

    // FIXME: verify we read the whole thing

    zco_stop_receiving(*sdr, &reader);
    sdr_end_xn(*sdr);

    syslog(LOG_INFO, "setting '%s' to: %s", resource_name, value);

    bionet_set_resource_by_name(resource_name, value);

    free(resource_name);
    free(value);
}




int main(int argc, char *argv[]) {
    char *ownEid;
    Sdr sdr;

    int option_index = 0;

    static struct option longopts[] = {
        { "help",       no_argument,        0, 'h' },
        { 0, 0, 0, 0 }
    };

    while(1) {
        int c;
        c = getopt_long(argc, argv, "h", longopts, &option_index);
        if (c == -1) break;

        switch (c) {
            case 'h': {
                usage();
                exit(0);
            }

            default: {
                usage();
                exit(1);
            }
        }
    }

    if ((optind+1) != argc) {
        fprintf(stderr, "no EID given!\n");
        usage();
        exit(1);
    }

    ownEid = argv[optind];

    setbuf(stdout, NULL);

    if (bionet_connect() < 0) {
        fprintf(stderr, "error connecting to bionet\n");
        exit(1);
    }

    if (bp_attach() < 0) {
        fprintf(stderr, "can't attach to BP\n");
        return 1;
    }

    if (bp_open(ownEid, &sap) < 0) {
        fprintf(stderr, "can't open endpoint '%s'\n", ownEid);
        return 1;
    }

    sdr = bp_get_sdr();
    signal(SIGINT, handleQuit);
    running = 1;

    // FIXME: background here

    syslog(LOG_INFO, "ready\n");

    while (running) {
        BpDelivery  dlv;

        if (bp_receive(sap, &dlv, BP_BLOCKING) < 0) {
            syslog(LOG_ERR, "bundle reception failed\n");
            break;
        }

        switch (dlv.result) {
            case  BpPayloadPresent: {
                // got a bundle!
                handle_bundle(&sdr, &dlv);
                break;
            }

            case BpReceptionTimedOut: {
                syslog(LOG_WARNING, "ION event: Reception timed out");
                break;
            }

            case BpReceptionInterrupted: {
                syslog(LOG_WARNING, "ION event: Reception interrupted");
                break;
            }

            default: {
                syslog(LOG_WARNING, "ION event: unknown delivery type %d", dlv.result);
                break;
            }
        }

        bp_release_delivery(&dlv, 1);
    }

    bp_close(sap);
    writeErrmsgMemos();
    bp_detach();
    syslog(LOG_DEBUG,"Stopping bionet-bp-commanding-proxy.");
    return 0;
}

