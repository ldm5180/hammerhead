
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <stdio.h>

#include "bpstats2-hab.h"

void usage() {
    fprintf(stderr, 
        "'bpstats2-hab' publishes bpstats2 bundles as Bionet traffic.\n"
        "\n"
        "Reads a bundle from EID sent by bpstats2; parses the data in\n"
        "that bundle and publishes it as bionet data."
        "\n"
        "usage:  random-hab [OPTIONS] <eid>\n"
        "\n"
        " -h,--help                     Show this usage information.\n"
        " -v,--version                  Show the version number\n"
        " -e,--require-security         Require security\n"
        " -i,--id <ID>                  Use ID as the HAB-ID (defaults to\n"
        "                               hostname if omitted).\n"
        " -s,--security-dir <dir>       Directory containing security certificates\n"
        " -k,--smkey <smkey>            Shared Memory key to use to connect to ION\n"
        "\n"
        "Security can only be required when a security directory has been specified.\n"
        "  bpstats2-hab <eid> [--security-dir <dir> [--require-security]]\n");
}
