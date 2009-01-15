
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
#include <time.h>
#include <unistd.h>


#include "cpod.h"


struct set_time_command
{
    uint8_t frame_marker;
    uint8_t size;
    uint8_t cmd;
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day_of_month;
    uint8_t month;
    uint8_t weekday;
    uint8_t year;
    uint8_t sequence_num;
    uint16_t crc;
} __attribute__ ((packed));


int set_time(int fd)
{
    int r;
    struct set_time_command command, response;

    struct tm *broken_time;
    time_t utime;

    command.frame_marker = FRAME_MARKER;
    command.size         = sizeof(command) - 4;
    command.cmd          = SET_TIME << CMD_OFFSET;
    command.sequence_num = next_command_number();

    utime = time(NULL);
    broken_time = gmtime(&utime);

    command.seconds = broken_time->tm_sec;
    command.minutes = broken_time->tm_min;
    command.hours = broken_time->tm_hour;
    command.day_of_month = broken_time->tm_mday;
    command.month = broken_time->tm_mon;
    command.weekday = broken_time->tm_wday;
    command.year = broken_time->tm_year;

    command.crc = g_htons(crcccitt((char*)&command.cmd, command.size));

    r = reliable_write(fd, &command, sizeof(command));

    if (r != sizeof(command))
        return -1;

    g_usleep(5*1000*1000);

    r = patient_read(fd, &response, sizeof(response));

    g_debug("r = %d", r);

    if (r == -1 ||
        verify_response((char*)&command, (char*)&response))
        return -2;

    return 0;

}