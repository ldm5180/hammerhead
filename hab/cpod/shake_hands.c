
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


int shake_hands( int fd )
{
    /*  Confusing - the docs say that the cpod initiates this conversation, but
     *  I can't see how it does this.  There isn't anything coming out of the
     *  cpod after connecting.  Wierd.
     */

    int r;
    char cmd[7], response[7];
    uint16_t data_checksum;

    cmd[0] = SYNC;
    cmd[1] = FRAME_MARKER;
    cmd[2] = (uint8_t) 2;
    cmd[3] = HANDSHAKE << 4;
    cmd[4] = next_command_number();

    data_checksum = g_htons(crcccitt(&cmd[3], 2));

    memcpy(&cmd[5], &data_checksum, 2);


    r = write(fd, cmd, 7);

    if(r != 7)
    {
        g_log("", G_LOG_LEVEL_CRITICAL, "Can't write to the file descriptor: %s", strerror(errno));
        return -1;
    }
    else
        g_log("", G_LOG_LEVEL_DEBUG, "Success!!");

    r = read(fd, response, 7);

    if(r == -1)
    {
        g_log("", G_LOG_LEVEL_CRITICAL, "Can't read from the file descriptor: %s", strerror(errno));
        return -1;
    }
    else
    {
        g_debug("Success!!: %c %c %c", response[0], response[1], response[2]);
    }

    
    return 0;    
}
