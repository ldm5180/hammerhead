
#include <ctype.h>
#include <stdio.h>

#include "bdp.h"


// print a hex & ascii dump of the packet
void bdp_hexdump(uint8_t *packet, int packet_size) {
    char hex[60];
    char ascii[20];
    int byte, hex_dst, ascii_dst;

    hex_dst = 0;
    ascii_dst = 0;

    for (byte = 0; byte < packet_size; byte ++) {
        sprintf(&hex[hex_dst], "%02X ", packet[byte]);
        hex_dst += 3;

        if (isprint(packet[byte])) {
            sprintf(&ascii[ascii_dst], "%c", packet[byte]);
        } else {
            sprintf(&ascii[ascii_dst], ".");
        }
        ascii_dst ++;

        if (byte % 16 == 15) {
            printf("    %s |%s|\n", hex, ascii);
            hex_dst = 0;
            ascii_dst = 0;
        } else if (byte % 8 == 7) {
            sprintf(&hex[hex_dst], " ");
            hex_dst ++;
            sprintf(&ascii[ascii_dst], " ");
            ascii_dst ++;
        }
    }

    if (byte % 16 != 0) {
        printf("    %-49s |%-17s|\n", hex, ascii);
    }
}

