
//
// Copyright (C) 2004-2009, Regents of the University of Colorado.
//


#include <inttypes.h>

uint8_t sum8(char* data, int size)
{
    int i;
    uint8_t checksum = 0x00;

    for(i = 0; i < size; i++)
        checksum += data[i];

    return checksum;
}
