
// Copyright (c) 2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef ENV_MSG_H
#define ENV_MSG_H

typedef nx_struct env_general_msg
{
    nx_uint16_t node_id;
    nx_uint16_t volt;
    nx_uint16_t temp;
    nx_uint16_t photo;
} env_general_msg_t;

enum
{
    /* msgs from nodes */
    COL_ENV_GENERAL_MSG  = 0x11,

    /* msgs from gateway */
    AM_ENV_GENERAL_MSG   = 0x21,
};
#endif /* ENV_MSG_H */
 
