
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

#ifndef MMOD_MSG_H
#define MMOD_MSG_H

typedef nx_struct mmod_general_msg
{
    nx_uint16_t node_id;
    nx_uint16_t accel_x;
    nx_uint16_t accel_y;
    nx_uint16_t accel_flags;
    nx_uint32_t tv_sec;
    nx_uint32_t tv_usec;
    nx_uint32_t offset;
} mmod_general_msg_t;


typedef nx_struct mmod_settings_msg
{
    nx_uint16_t node_id;
    nx_uint16_t thres_accel;
    nx_uint16_t sample_interval;
    nx_uint16_t num_accel_samples;
    nx_uint16_t accel_sample_interval;
    nx_uint16_t heartbeat_time;
    nx_uint16_t accel_flags;
    nx_uint32_t tv_sec;
    nx_uint32_t tv_usec;
} mmod_settings_msg_t;


#define ACCEL_FLAG_X 0x0001
#define ACCEL_FLAG_Y 0x0002

enum
{
    /* msgs from nodes */
    COL_MMOD_GENERAL_MSG  = 0x11,
    COL_MMOD_SETTINGS_MSG = 0x12,

    /* msgs from gateway */
    AM_MMOD_GENERAL_MSG   = 0x21,
    DIS_MMOD_SETTINGS_MSG = 0x22,

    /* msgs from host or node */
    AM_MMOD_SETTINGS_MSG  = 0x32,
};
#endif /* MMOD_MSG_H */
