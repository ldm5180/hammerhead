
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <glib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>


#define WRITE_ATTEMPTS 5
#define WRITE_TIMEOUT 500000

#define READ_ATTEMPTS 2
#define READ_TIMEOUT 2


size_t reliable_write(int fd, const void* buffer, size_t size)
{
    int i;
    int attempts = 0;

    do
    {
        fd_set readers;
        struct timeval tv = { 0, WRITE_TIMEOUT };

        i = write(fd, buffer, size);

        if (i != size)
        {
            g_critical("Write returned %d: %s", i, strerror(errno));
            return -1;
        }

        FD_ZERO(&readers);
        FD_SET(fd, &readers);

        i = select(fd+1, &readers, NULL, NULL, &tv);

        if (i == -1)
        {
            g_critical("Select returned -1: %s", strerror(errno));
            return -1;
        }
        else if (i == 0)
        {
            attempts++;
            if (attempts == WRITE_ATTEMPTS)
                return -2;
        }
        else if (FD_ISSET(fd, &readers))
            return size;
    } while (i > -1);

    return -1;
}


size_t patient_read(int fd, void *buffer, size_t count)
{
    int i;
    int attempts = 0;
    int size = count;
    
    for (;;)
    {
        fd_set readers;
        struct timeval tv = { READ_TIMEOUT, 0 };

        FD_ZERO(&readers);
        FD_SET(fd, &readers);

        i = select(fd+1, &readers, NULL, NULL, &tv);

        if (i == -1)
        {
            g_critical("Select returned -1: %s", strerror(errno));
            return -1;
        }
        else if (i == 0)
        {
            attempts++;
            if (attempts == READ_ATTEMPTS)
                return -2;
        }
        else if (FD_ISSET(fd, &readers))
        {
            int j;

            j = read(fd, buffer+count-size, size);

            if (j == -1)
                return -1;

            size -= j;

            if (size == 0)
                return count;
        }
    }

    return -1;
}
