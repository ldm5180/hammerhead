
// Copyright (c) 2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef ENV_MESSAGE_H
#define ENV_MESSAGE_H

#include <inttypes.h>

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

#endif /* ENV_MESSAGE_H */
