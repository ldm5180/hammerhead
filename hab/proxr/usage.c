#include "sim-hab.h"


//
// display usage
//
void usage(void)
{
    fprintf(stderr, 
            "\ncgba-sim-proxr-hab must be ran with one option. -p followed\n"
            "by a file location.\n"
            "\n"
            "usage: cgba-sim-proxr-hab [OPTIONS]\n"
            " -?, --help             Show this information.\n"
            " -v, --version          Show the version number.\n"
            " -i, --id <ID>          Use ID as the HAB-ID (defaults to\n"
            "                        hostname if omitted).\n"
            " -p --proxr <FILE>      Give the ProXR file location to open.\n"
            );
}
