// proxrport.c - Serial Port Handler

#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "proxrport.h"

static int fd = 0;

/** @brief attempts to open USB0 port and configures terminal attributes
 *  @param void
 *  @return file descriptor of opened port
 */
int proxr_connect(char *conn)
{
    struct termios my_termios;

    fd = open(conn, O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd == -1)
    {
        printf("open_port: open error %d: %s\n", errno, strerror(errno));
    }
    else
    {
        fcntl(fd, F_SETFL, 0);
    }

    tcgetattr(fd, &my_termios);
    tcflush(fd, TCIFLUSH);

    my_termios.c_cflag |= B115200 | CREAD | CLOCAL | CS8 | HUPCL;
    cfsetispeed(&my_termios, B115200);
    cfsetospeed(&my_termios, B115200);

    tcsetattr(fd, TCSADRAIN, &my_termios);

    return fd;
}
/** @brief attempts to write parameter
 *  @param command to write as ASCII character code
 *  @return the return value of the write() call
 */
int write_port(char cmd)
{
    int iOut;
    if(fd < 1)
    {
        printf("write_port:port is not open\n");
        return -1;
    }

    iOut = write(fd, &cmd, 1);
    if(iOut < 0)
    {
        printf("write_port: write error %d: %s\n", errno, strerror(errno));
    }

    return iOut;
}

/** @brief attempts a read from the open port
 *  @param the result of read to be written in
 *  @return the return value of the read() call
 */
int read_port(char *result)
{
    int iIn;
    if(fd < 1)
    {
        printf("read_port:port is not open\n");
        return -1;
    }
    iIn = read(fd, result, 1);
    if(iIn < 0)
    {
        if(errno == EAGAIN)
        {
            return 0; //assume command generated no command
        }
        else
        {
            printf("read_port: read error %d: %s\n", errno, strerror(errno));
        }
    }
    else
    {
        return iIn;
    }
    return 0;
}

/** @brief closes open port
 *  @param void
 *  @return void
 */
void proxr_disconnect(void)
{
    if(fd > 0)
    {
        close(fd);
    }
}
