
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <glib.h>
#include <unistd.h>


#include "cpod.h"


struct avail_ops_command
{
    uint8_t frame_marker;
    uint8_t size;
    uint8_t cmd;
    uint8_t sequence_num;
    uint16_t crc;
} __attribute__ ((packed));

struct avail_ops_response
{
    uint8_t frame_marker;
    uint8_t size;
    uint8_t cmd;
    uint8_t opcodes[9];
    uint8_t sequence_num;
    uint16_t crc;
} __attribute__ ((packed));


int available_opcodes(int fd)
{
    int r;
    struct avail_ops_command command;
    struct avail_ops_response response;

    command.frame_marker = FRAME_MARKER;
    command.size         = 0x02;
    command.cmd          = AVAILABLE_OPCODES << CMD_OFFSET;
    command.sequence_num = next_command_number();
    command.crc          = g_htons(crcccitt((char*)&command.cmd, 2));

    r = reliable_write(fd, &command, sizeof(command));

    if (r != sizeof(command))
        return -1;

    r = patient_read(fd, &response, sizeof(response));

    if (r == -1 ||
        verify_response((char*)&command, (char*)&response))
        return -2;

    return 0;
}
