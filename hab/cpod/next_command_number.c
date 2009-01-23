
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include "cpod.h"


uint8_t next_command_number()
{
    static uint8_t sequence_number = 1;

    sequence_number++;

    return (sequence_number - 1);
}
