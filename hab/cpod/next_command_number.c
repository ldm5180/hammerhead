
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "cpod.h"


uint8_t next_command_number()
{
    static uint8_t sequence_number = 1;

    sequence_number++;

    return (sequence_number - 1);
}
