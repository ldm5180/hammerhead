
//
// Copyright (C) 2004-2008, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of GNU General Public License version 2, as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
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
