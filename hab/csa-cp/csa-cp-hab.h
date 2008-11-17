
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


// for the FILE data type
#include <stdio.h>


#include "hardware-abstractor.h"

#include "serial.h"


#define Max(a, b)  (((a) > (b)) ? (a) : (b))




extern bionet_hab_t *this_hab;




// 
//  FUNCTION:  Probes for the CSA-CP device for 10 seconds.
//
// ARGUMENTS:  'device_file' is the name of the serial port to probe on.
//             On Unix this would probably be something like "/dev/ttyS0"
//             or "/dev/ttyUSB3".  On Windows this would probably be
//             something like "COM2".
//
//   RETURNS:  If it finds a CSA-CP it returns the file descriptor of the
//             serial port connected to the CSA-CP.  If it doesnt find one,
//             it returns -1.
//

serial_handle_t csa_cp_probe(const char *device_file);




//
//  FUNCTION:  Tries to ping the CSA-CP.
//
// ARGUMENTS:  'serial_handle' is the serial handle previously returned
//             from serial_open() or csa_cp_probe().
//
//   RETURNS:  Returns TRUE (1) if the CSA-CP responded to the ping, FALSE
//             (0) if not.
//

int csa_cp_ping(serial_handle_t serial_handle);




// 
//  FUNCTION:  Reads the number of files stored on the CSA-CP.
//
// ARGUMENTS:  'serial_handle' is the serial port handle connected to the
//             CSA-CP, previously returned from csa_cp_probe().
//
//   RETURNS:  If it finds a CSA-CP it returns the file descriptor of the
//             serial port connected to the CSA-CP.  If it doesnt find one,
//             it returns -1.
//

int get_num_files(serial_handle_t serial_handle);




// 
//  FUNCTION:  Downloads a session from the CSA-CP, reports it to Bionet,
//             and optionally saves the raw data to a file on disk.
//
// ARGUMENTS:  'serial_handle' is the serial handle connected to the
//             CSA-CP, previously returned from csa_cp_probe().
//             'session_number' is the CSA-CP session to download.
//             'record_raw_data' is a boolean (0 for false, non-zero for
//             true) that determines whether to save the raw data to a file
//             or not.
//
//   RETURNS:  0 on success, -1 on failure.
//

int download_session(serial_handle_t serial_handle, int session_number, int record_raw_data);




// 
//  FUNCTION:  Removes the current session from the CSA-CP.
//
// ARGUMENTS:  'serial_handle' is the serial port handle connected to the
//             CSA-CP.
//
//   RETURNS:  0 on success, -1 on failure.
//

int clear_csa(serial_handle_t serial_handle);




// if the CSA-CP had any actuators we wanted to expose, this callback would
// let us know when someone wants to change them
void cb_set_resource(const char *node_id, const char *resource_id, const char *value);


// helper functions not directly related to Bionet or the CSA-CP
void print_help(void);
void make_shutdowns_clean(void);

