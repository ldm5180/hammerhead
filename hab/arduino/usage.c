#include "sim-hab.h"


//
// display usage
//
void usage(void)
{
    fprintf(stderr, 
            "\ncgba-sim-arduino-hab must be ran with one option. -a followed\n"
            "by a file location.\n"
            "\n"
            "usage: cgba-sim-arduino-hab [OPTIONS]\n"
            " -?, --help             Show this information.\n"
            " -v, --version          Show the version number.\n"
            " -i, --id <ID>          Use ID as the HAB-ID (defaults to\n"
            "                        hostname if omitted).\n"
            " -a --arduino <FILE>    Give the Arduino file location to open.\n"
            );
}
