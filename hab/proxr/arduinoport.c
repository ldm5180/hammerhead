// arduinoport.c - Serial Port Handler

#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "arduinoport.h"

static int fd = 0;

/** @brief attempts to open paramater file name s
 *  @param void
 *  @return file descriptor of opened port
 */
int arduino_connect(char *conn)
{
    struct termios my_termios;

    fd = open(conn, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
    if(fd == -1)
    {
        printf("open_port: open error %d: %s\n", errno, strerror(errno));
    }
    else
    {
        fcntl(fd, F_SETFL);
    }

    tcgetattr(fd, &my_termios);
    tcflush(fd, TCIOFLUSH);

    my_termios.c_cflag |= B9600 | CREAD | CLOCAL | CS8;
    my_termios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    cfsetispeed(&my_termios, B9600);
    cfsetospeed(&my_termios, B9600);

    tcsetattr(fd, TCSADRAIN, &my_termios);

    return fd;
}
/** @brief attempts to write parameter
 *  @param command to write as ASCII character code
 *  @return the return value of the write() call
 */
int arduino_write(char cmd)
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

//
// read from arduino device until specified char is reached
//
int arduino_read_until(char* buf, char until)
{
    int n;
    char b[1];
    int i = 0;
    do
    {
        n = read(fd, b, 1);
        if(n < 0)
        {
            printf("arduino_read_until: read error %d: %s\n", errno, strerror(errno));
        }
        else if(n == 0)
        {
            //hace nada
        }
        else
        {
            buf[i] = b[0];
            i++;
        }
     } while(b[0] != until);

     buf[--i] = '\0';

    return 0;
}


int get_arduino_fd(void)
{
    return fd;
}

/** @brief closes open port
 *  @param void
 *  @return void
 */
void arduino_disconnect(void)
{
    if(fd > 0)
    {
        close(fd);
    }
}
