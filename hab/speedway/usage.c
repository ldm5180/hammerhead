
#include <stdio.h>

void usage(void) {
    printf("usage: speedway [OPTIONS] TARGET\n\
\n\
TARGET is the hostname or IP of a Speedway device.\n\
\n\
OPTIONS is zero or more of these:\n\
\n\
    --help           Show this help.\n\
\n\
    --id ID          Set the HAB-ID to ID.  Defaults to the local hostname\n\
                     if omitted.\n\
\n\
    --gpi-delay N    After a GPI1 event, wait N milliseconds before\n\
                     starting to scan for tags.  Defaults to 0.\n\
\n\
    --show-messages  Enable printing of all messages between the HAB and\n\
                     the Speedway device.  Defaults to off.\n\
");
}

