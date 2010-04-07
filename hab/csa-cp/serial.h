
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef SERIAL_H
#define SERIAL_H




// 
// This is an abstraction library that attempts to make serial port
// programming portable between Windows and Linux.
//




#if defined(LINUX) || defined(MACOSX)
    typedef int serial_handle_t;
    #define INVALID_HANDLE_VALUE (-1)
#endif

#ifdef WINDOWS
    #include <windows.h>
    typedef HANDLE serial_handle_t;
#endif




// 
//  FUNCTION:  Opens the serial port and configures the RS-232 settings for
//             communication with the CSA-CP (4800 baud, 8 data bits, no
//             parity, 1 stop bit).
//
// ARGUMENTS:  'device' is the name of the serial device to open.  On Unix
//             this would be something like "/dev/ttyS0" or "/dev/ttyUSB3".
//             On Windows this would be something like "COM2".
//
//   RETURNS:  On success it returns the open serial handle.
//             On failure it returns -1 and sets errno.
//
//     FIXME:  Should let the caller provide baud rate, parity, data bits,
//             and stop bits.
//

serial_handle_t serial_open(const char *device);




// 
//  FUNCTION:  Discards all buffered data to and from the serial port.
//
// ARGUMENTS:  'serial_handle' is the serial port handle to flush (returned
//             from serial_open()).
//
//   RETURNS:  On success it returns 0, on failure it returns -1.
//

int serial_flush(serial_handle_t serial_handle);




// 
//  FUNCTION:  Reads data from a serial port, with timeout.
//
// ARGUMENTS:  'serial_handle' is the serial port handle to read on.
//             'buffer' is where to put the data it reads, and 'count' is
//             the max number of bytes to read.
//             'usec_timeout' is the max number of microseconds to read.
//
//   RETURNS:  On success it returns the number of bytes read.
//             On failure it sets errno and returns -1.
//

int serial_read(serial_handle_t serial_handle, void *buffer, int count, int usec_timeout);




// 
//  FUNCTION:  Writes data to a serial port.
//
// ARGUMENTS:  'serial_handle' is the serial port handle to write to.
//             'buffer' is what data to write, and 'count' is how much data
//             to write.
//
//   RETURNS:  On success it returns the number of bytes written.
//             On failure it sets errno and returns -1.
//

int serial_write(serial_handle_t serial_handle, const void *buffer, int count);




// 
//  FUNCTION:  Writes one byte to a serial port.  This is sometimes a more
//             convenient interface than serial_write(), above.
//
// ARGUMENTS:  'serial_handle' is the serial port file descriptor to write to.
//             'char' is the byte to write.
//
//   RETURNS:  On success it returns the number of bytes written.
//             On failure it sets errno and returns -1.
//

int serial_write_byte(serial_handle_t serial_handle, char c);




// 
//  FUNCTION:  Closes the serial port.
//
// ARGUMENTS:  The serial port handle to close.
//
//   RETURNS:  Nothing.
//

void serial_close(serial_handle_t serial_handle);




#endif  // SERIAL_H

