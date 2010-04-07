
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <inttypes.h>

uint8_t sum8(char* data, int size)
{
    int i;
    uint8_t checksum = 0x00;

    for(i = 0; i < size; i++)
        checksum += data[i];

    return checksum;
}
