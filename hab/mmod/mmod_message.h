
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef MMOD_MESSAGE_H
#define MMOD_MESSAGE_H

#include <inttypes.h>

#define ACCEL_FLAG_X 0x0001
#define ACCEL_FLAG_Y 0x0002

/**
 * Process the incoming "general" message
 * 
 * Pull apart the message, ensure it is valid, and send it off to the NAG
 * 
 * @param[in] msg Ptr to start of the message
 * @param[in] len Length of the message
 *
 * @return 0 Success
 * @return 1 Failure (bad message. probably out of sync)
 */
int msg_gen_process(uint8_t *msg, ssize_t len);


/**
 * Process the incoming "settings" message
 * 
 * Pull apart the message, ensure it is valid, and send it off to the NAG
 * 
 * @param[in] msg Ptr to start of the message
 * @param[in] len Length of the message
 *
 * @return 0 Success
 * @return 1 Failure (bad message. probably out of sync)
 */
int msg_settings_process(uint8_t *msg, ssize_t len);

#endif /* MMOD_MESSAGE_H */
