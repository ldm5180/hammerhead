#include "translator.h"


//
// display usage
//
void usage(void)
{
    fprintf(stderr, 
            "\ntranslator client hab halfbreed\n"
            "\n"
            "usage: cgba-sim-proxr-hab [OPTIONS]\n"
            " -?, --help             Show this information.\n"
            " -v, --version          Show the version number.\n"
            " -i, --id <ID>          Use ID as the HAB-ID (defaults to\n"
            "                        hostname if omitted).\n"
            );
}
