
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
