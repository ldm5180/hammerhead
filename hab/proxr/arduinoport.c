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

    my_termios.c_cflag |= B9600 | CREAD | CLOCAL | CS8;
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
    char b[1];
    int i=0;
    do {
        int n = read(fd, b, 1);  // read a char at a time
        if( n==-1) return -1;    // couldn't read
        if( n==0 ) {
            //usleep( 10 * 1000 ); // wait 10 msec try again
            continue;
        }
        buf[i] = b[0]; i++;
    } while( b[0] != until );

    buf[i] = 0;  // null terminate the string
    return 0;
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
