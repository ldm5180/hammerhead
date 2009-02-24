
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef MMOD_MSG_H
#define MMOD_MSG_H

typedef nx_struct mmod_general_msg
{
    nx_uint16_t node_id;
    nx_uint16_t volt;
    nx_uint16_t temp;
    nx_uint16_t photo;
    nx_uint16_t accel_x;
    nx_uint16_t accel_y;
    nx_uint16_t accel_flags;
} mmod_general_msg_t;


typedef nx_struct mmod_accel_msg
{
    nx_uint16_t node_id;
    nx_uint16_t accel_x;
    nx_uint16_t accel_y;
} mmod_accel_msg_t;


typedef nx_struct mmod_settings_msg
{
    nx_uint16_t node_id;
    nx_uint16_t thres_light;
    nx_uint16_t thres_accel;
    nx_uint16_t thres_temp;
    nx_uint16_t thres_volt;
    nx_uint16_t sample_interval;
    nx_uint16_t num_accel_samples;
    nx_uint16_t accel_sample_interval;
    nx_uint16_t heartbeat_time;
    nx_uint16_t accel_flags;
} mmod_settings_msg_t;


#define ACCEL_FLAG_X 0x0001
#define ACCEL_FLAG_Y 0x0002

enum
{
    /* msgs from nodes */
    COL_MMOD_GENERAL_MSG  = 0x11,
    COL_MMOD_ACCEL_MSG    = 0x12,
    COL_MMOD_SETTINGS_MSG = 0x13,

    /* msgs from gateway */
    AM_MMOD_GENERAL_MSG   = 0x21,
    AM_MMOD_ACCEL_MSG     = 0x22,
    DIS_MMOD_SETTINGS_MSG = 0x23,

    /* msgs from host or node */
    AM_MMOD_SETTINGS_MSG  = 0x33,
};
#endif /* MMOD_MSG_H */
