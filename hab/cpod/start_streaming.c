
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


#include <errno.h>
#include <glib.h>
#include <string.h>
#include <unistd.h>

#include "cpod.h"


int start_streaming(int fd)
{
    const int SIZE = 6;
    int r;
    char cmd[SIZE], response[SIZE];
    uint16_t checksum;

    cmd[0] = FRAME_MARKER;
    cmd[1] = (uint8_t) 2;
    cmd[2] = START_STREAMING << CMD_OFFSET;
    cmd[3] = next_command_number();


    checksum = g_htons(crcccitt(&cmd[2], 2));
    memcpy(&cmd[4], &checksum, 2);


    r = reliable_write(fd, cmd, SIZE);

    if (r != SIZE)
    {
        g_critical("Can't write to the file descriptor: %s", strerror(errno));
        return -1;
    }

    g_usleep(100 * 1000);

    r = 0;
    while (r != SIZE)
    {
        int i;
        i = read(fd, response+r, SIZE);

        if (i < 0 && errno != EAGAIN)
                break;

        r += i;
    }

    if (r != SIZE ||
        verify_response(cmd, response))
    {
        g_critical("Bad read!!: verify_response(...) = %d, r = %d, %s",
                   verify_response(cmd, response), r, strerror(errno));
        return -2;
    }


    g_debug("Started conversation with CPOD!!");

    return 0;
}
