#include "translator.h"


//
// display usage
//
void usage(void)
{
    fprintf(stderr, 
            "\nTranslator Clab\n"
            "Two required arguments -p and -d to provide DMMs and proxrs hab id\n"
            "Example -p cgba5-gse-1 -d cgba4\n"
            "\n"
            "usage: cgba-sim-proxr-hab [OPTIONS]\n"
            " -?, --help              Show this information.\n"
            " -v, --version           Show the version number.\n"
            " -p, --proxr <hab_id>    Used to provide proxr's hab id.\n"
            " -d, --dmm <hab_id>      Used to provide dmm's hab id.\n"
            " -i, --id <ID>           Use ID as the HAB-ID (defaults to\n"
            "                         hostname if omitted).\n"
            );
}
