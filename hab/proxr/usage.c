#include "sim-hab.h"


//
// display usage
//
void usage(void)
{
    fprintf(stderr, 
            "\nsim-hab must be ran with two options. -p and -a both followed\n"
            "by a file location. To determine the file locations run the\n"
            "findPort program.\n"
            "\n"
            "usage: sim-hab [OPTIONS]\n"
            " -?, --help             Show this information.\n"
            " -v, --version          Show the version number.\n"
            " -i, --id <ID>          Use ID as the HAB-ID (defaults to\n"
            "                        hostname if omitted).\n"
            " -p --proxr <FILE>      Give the ProXR file location to open.\n"
            " -a --arduino <FILE>    Give the Arduino file location to open.\n"
            );
}
