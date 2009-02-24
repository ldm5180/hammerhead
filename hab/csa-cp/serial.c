
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#if  defined(LINUX) || defined(MACOSX)
    #include <termios.h>
#endif

#ifdef WINDOWS
    #include <windows.h>
#endif

#include <glib.h>

#include "serial.h"




serial_handle_t serial_open(const char *device) {
    serial_handle_t serial_handle;

#if defined(LINUX) || defined(MACOSX)
    struct termios tio;
    int r;

    serial_handle = open(device, (O_RDWR | O_NONBLOCK));
    if (serial_handle < 0) {
        g_log("", G_LOG_LEVEL_DEBUG, "error opening serial port '%s': %s", device, strerror(errno));
        return INVALID_HANDLE_VALUE;
    }

    r = tcgetattr(serial_handle, &tio);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "error getting termios of '%s': %s", device, strerror(errno));
        close(serial_handle);
        return INVALID_HANDLE_VALUE;
    }

    tio.c_iflag = 0;
    tio.c_oflag = 0;
    tio.c_cflag = (CLOCAL | CREAD | CS8);
    tio.c_lflag = 0;
    tio.c_cc[VTIME] = 0;
    tio.c_cc[VMIN] = 1;

    r = cfsetispeed(&tio, B4800);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "error setting input speed to 4.8K: %s", strerror(errno));
        close(serial_handle);
        return INVALID_HANDLE_VALUE;
    }

    r = cfsetospeed(&tio, B4800);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "error setting output speed to 4.8K: %s", strerror(errno));
        close(serial_handle);
        return INVALID_HANDLE_VALUE;
    }

    r = tcflush(serial_handle, TCIOFLUSH);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "error flushing '%s': %s", device, strerror(errno));
        close(serial_handle);
        return INVALID_HANDLE_VALUE;
    }

    r = tcsetattr(serial_handle, TCSANOW, &tio);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "error setting termios of '%s': %s", device, strerror(errno));
        close(serial_handle);
        return INVALID_HANDLE_VALUE;
    }

    return serial_handle;
#endif


#ifdef WINDOWS
    DCB dcb;
    int success;

    serial_handle = CreateFile(
        device,
        (GENERIC_READ | GENERIC_WRITE),
        0,      // dont share
        NULL,   // no security
        OPEN_EXISTING,
        0,      // no overlapped i/o
        NULL    // no template for the serial port
    );

    if (serial_handle == INVALID_HANDLE_VALUE) {
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0,
            NULL
        );
        g_warning("cannot open '%s': %s", device, (char *)lpMsgBuf);
        LocalFree(lpMsgBuf);
        return INVALID_HANDLE_VALUE;
    }

    success = GetCommState(serial_handle, &dcb);
    if (!success) {
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0,
            NULL
        );
        g_warning("cannot get comm state for '%s': %s", device, (char *)lpMsgBuf);
        LocalFree(lpMsgBuf);
        CloseHandle(serial_handle);
        return INVALID_HANDLE_VALUE;
    }

    dcb.BaudRate = CBR_4800;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    success = SetCommState(serial_handle, &dcb);
    if (!success) {
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0,
            NULL
        );
        g_warning("cannot set comm state of '%s' to 4800n81: %s", device, (char *)lpMsgBuf);
        LocalFree(lpMsgBuf);
        CloseHandle(serial_handle);
        return INVALID_HANDLE_VALUE;
    }

    return serial_handle;
#endif

}


int serial_flush(serial_handle_t serial_handle) {

#if defined(LINUX) || defined(MACOSX)
    int r;

    r = tcflush(serial_handle, TCIOFLUSH);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "error flushing serial port: %s", strerror(errno));
        return -1;
    }

    return 0;
#endif

#ifdef WINDOWS
    int success;

    success = PurgeComm(serial_handle, (PURGE_RXCLEAR | PURGE_TXCLEAR));
    if (!success) {
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0,
            NULL
        );
        g_warning("error flushing serial port: %s", (char *)lpMsgBuf);
        LocalFree(lpMsgBuf);
        return -1;
    }

    return 0;
#endif

}


int serial_read(serial_handle_t serial_handle, void *buffer, int count, int usec_timeout) {

#if defined(LINUX) || defined(MACOSX)
    int r;
    int bytes_read = 0;
    struct timeval start;
    int usec_remaining;

    r = gettimeofday(&start, NULL);
    if (r < 0) {
        g_warning("gettimeofday(): %s", strerror(errno));
        return -1;
    }

    do {
        fd_set readers;
        struct timeval now;
        struct timeval timeout_so_far;
        int usec_so_far;
        struct timeval timeout;

        FD_ZERO(&readers);
        FD_SET(serial_handle, &readers);

        r = gettimeofday(&now, NULL);
        if (r < 0) {
            g_warning("gettimeofday(): %s", strerror(errno));
            return -1;
        }

        timeout_so_far.tv_sec = now.tv_sec - start.tv_sec;
        timeout_so_far.tv_usec = now.tv_usec - start.tv_usec;
        if (timeout_so_far.tv_usec < 0) {
            timeout_so_far.tv_sec --;
            timeout_so_far.tv_usec += 1000 * 1000;
        }

        usec_so_far = (timeout_so_far.tv_sec * 1000 * 1000) + timeout_so_far.tv_usec;
        usec_remaining = usec_timeout - usec_so_far;

        if (usec_remaining < 0) {
            usec_remaining = 0;
        }

        timeout.tv_sec = usec_remaining / (1000*1000);
        timeout.tv_usec = usec_remaining % (1000*1000);

#if 0
        g_debug(
            "usec_timeout=%d, start=%d.%06d, now=%d.%06d, usec_so_far=%d, usec_remaining=%d, timeout=%d.%06d",
            usec_timeout,
            (int)start.tv_sec, (int)start.tv_usec,
            (int)now.tv_sec, (int)now.tv_usec,
            usec_so_far,
            usec_remaining,
            (int)timeout.tv_sec, (int)timeout.tv_usec
        );
#endif

        r = select(serial_handle + 1, &readers, NULL, NULL, &timeout);

        if (r < 0) {
            g_log("", G_LOG_LEVEL_WARNING, "error selecting on serial port: %s", strerror(errno));
            return -1;
        }

        if (r == 0) {
            // timeout
            return bytes_read;
        }


        //
        // if we get here, the serial port is readable
        //


        r = read(serial_handle, &((char *)buffer)[bytes_read], count);
        if (r < 0) {
            g_log("", G_LOG_LEVEL_WARNING, "error reading from serial port: %s", strerror(errno));
            return -1;
        }

        count -= r;
        bytes_read += r;
    } while ((count > 0) && (usec_remaining > 0));

    return bytes_read;
#endif


#ifdef WINDOWS
    int success;
    DWORD bytes_read;
    COMMTIMEOUTS timeouts;

    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = usec_timeout / 1000.0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;

    success = SetCommTimeouts(serial_handle, &timeouts);
    if (!success) {
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0,
            NULL
        );
        g_warning("error setting serial port timeout: %s", (char *)lpMsgBuf);
        LocalFree(lpMsgBuf);
        return -1;
    }

    success = ReadFile(serial_handle, buffer, count, &bytes_read, NULL);
    if (!success) {
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0,
            NULL
        );
        g_warning("error reading serial port: %s", (char *)lpMsgBuf);
        LocalFree(lpMsgBuf);
        return -1;
    }

    return bytes_read;
#endif

}


int serial_write(serial_handle_t serial_handle, const void *buffer, int count) {

#if defined(LINUX) || defined(MACOSX)
    return write(serial_handle, buffer, count);
#endif

#ifdef WINDOWS
    int success;
    DWORD bytes_written;

    success = WriteFile(serial_handle, buffer, count, &bytes_written, NULL);
    if (!success) {
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0,
            NULL
        );
        g_warning("error writing to serial port: %s", (char *)lpMsgBuf);
        LocalFree(lpMsgBuf);
        return -1;
    }

    return bytes_written;
#endif

}


int serial_write_byte(serial_handle_t serial_handle, char c) {
    return serial_write(serial_handle, &c, 1);
}


void serial_close(serial_handle_t serial_handle) {
#if defined(LINUX) || defined(MACOSX)
    close(serial_handle);
#endif

#ifdef WINDOWS
    CloseHandle(serial_handle);
#endif

}

