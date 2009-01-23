
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


/*  This checksum routine was taken from
    
    http://www.lammertbies.nl/comm/info/crc-calculation.html

    It was experimentally determined that the cpod uses the
    CRC-CCITT (0xffff) checksum routine
*/


#include "cpod.h"


#define P_CCITT 0x1021


static unsigned short crc_ccitt_table[256];


void init_crcccitt_table()
{
    int i, j;
    unsigned short crc, c;

    for (i=0; i<256; i++) {

        crc = 0;
        c   = ((unsigned short) i) << 8;

        for (j=0; j<8; j++) {

            if ( (crc ^ c) & 0x8000 ) crc = ( crc << 1 ) ^ P_CCITT;
            else                      crc =   crc << 1;

            c = c << 1;
        }

        crc_ccitt_table[i] = crc;
    }

}

uint16_t crcccitt(char* data, int size)
{
    uint16_t crc = 0xffff;

    while (size--)
    {
        unsigned short tmp;

        tmp = (crc >> 8) ^ (0x00ff & (unsigned short) *data++);
        crc = (crc << 8) ^ crc_ccitt_table[tmp];
    }

    return crc;
}
