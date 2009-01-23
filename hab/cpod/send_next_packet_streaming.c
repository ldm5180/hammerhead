
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <glib.h>
#include <unistd.h>


#include "cpod.h"


struct next_packet
{
    uint8_t frame_marker;
    uint8_t size;
    uint8_t cmd;
    uint8_t sequence_num;
    uint16_t crc;
} __attribute__ ((packed));

int send_next_packet_streaming(int fd)
{
    int r;
    struct next_packet command;

    command.frame_marker = FRAME_MARKER;
    command.size         = sizeof(command) - 4;
    command.cmd          = NEXT_PACKET_STREAMING << CMD_OFFSET;
    command.sequence_num = next_command_number();
    command.crc          = g_htons(crcccitt((char*)&command.cmd, command.size));

    r = reliable_write(fd, &command, sizeof(command));

    if (r != sizeof(command))
        return -1;

    return 0;
}
