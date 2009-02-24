
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <glib.h>

#include "hardware-abstractor.h"


#define Max(a, b)  (((a) > (b)) ? (a) : (b))


void print_menu(void);

void print_menu(void) {
    printf("\n");
    printf("\n        C        Establish serial connection");
    printf("\n        D        Download a selected specific session");
    printf("\n        Z        Clear data files on device");
    printf("\n        Q        Quit program and terminate connection");
    printf("\n\n        Enter command: ");
}



int main(int argc, char *argv[]) {

    char command = ' ';
    char cmdline[100];
    char *serial_port = "/dev/ttyUSB1";

    int connection_state = 0;
    int downloading = 0;
    int i;
    int len;
    int num_sessions = -1;
    int r;
    int ready_to_download = 0;
    int serial_fd;
    int session_desired;

    struct termios tio;


    bionet_log_context_t log_context = {
        destination: BIONET_LOG_TO_SYSLOG,
        log_limit: G_LOG_LEVEL_INFO
    };

    g_log_set_default_handler(bionet_glib_log_handler, &log_context);

    for (i = 1; i < argc; i ++) {
        if ((strcmp(argv[i], "-d") == 0) || 
            (strcmp(argv[i], "--device") == 0)) {
            i ++;
            serial_port = argv[i];
        } else {
            printf("unknown argument '%s'\n", argv[i]);
            exit(1);
        }
    }

    //
    // Open the serial port and configure for the CSA-CP device
    //

    serial_fd = open(serial_port, (O_RDWR | O_NONBLOCK));
    if (serial_fd < 0) {
        g_log("", G_LOG_LEVEL_DEBUG, "error opening serial port '%s': %s", serial_port, strerror(errno));
        return -1;
    }

    r = tcgetattr(serial_fd, &tio);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "error getting termios of '%s': %s", serial_port, strerror(errno));
        close(serial_fd);
        serial_fd = -1;
        exit(1);
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
        close(serial_fd);
        serial_fd = -1;
        exit(1);
    }

    r = cfsetospeed(&tio, B4800);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "error setting output speed to 4.8K: %s", strerror(errno));
        close(serial_fd);
        serial_fd = -1;
        exit(1);
    }

    r = tcflush(serial_fd, TCIFLUSH);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "error flushing '%s': %s", serial_port, strerror(errno));
        close(serial_fd);
        serial_fd = -1;
        exit(1);
    }

    r = tcsetattr(serial_fd, TCSANOW, &tio);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "error setting termios of '%s': %s", serial_port, strerror(errno));
        close(serial_fd);
        serial_fd = -1;
        exit(1);
    }

    printf("\n\t===========================");
    printf("\n\tEnsure CSA-CP device is OFF");
    printf("\n\t===========================");

    while (command != 'Q' && command != 'q') {

        print_menu();
        fgets(cmdline, 100, stdin);
        len = strlen(cmdline);
        command = cmdline[0];
        // printf("command = %c | len = %d | cmdline = %s\n", command, len, cmdline);

        switch(command) {

            case 'C':
            case 'c':

                // Loop while waiting for device to be turned ON

                while (connection_state != 2) {
                    int max_fd;
                    fd_set readers;
                    int r;
                    uint8_t buf = 0x11;

                    struct timeval timeout;


                   
                    if (connection_state == 0) {
                        //
                        // Send 0x11 twice a second and see if get response
                        //
                        buf = 0x11;
                        printf("\n\tAttempting to connect to the CSA-CP");
                    } else if (connection_state == 1) {
                        buf = 0x01;
                        printf("\n\tDetermining number of available sessions");
                    }

                    r = tcflush(serial_fd, TCIFLUSH);
                    if (r < 0) {
                        g_log("", G_LOG_LEVEL_WARNING, "error flushing '%s': %s", serial_port, strerror(errno));
                        close(serial_fd);
                        serial_fd = -1;
                        exit(1);
                    }

                    r = write(serial_fd, &buf, 1);
                    if (r < 0) {
                        g_log("", G_LOG_LEVEL_WARNING, "error writing to serial port: %s", strerror(errno));
                        exit(1);
                    }
                    if (r == 0) {
                        g_log("", G_LOG_LEVEL_WARNING, "write to the serial port returned 0");
                        exit(1);
                    }

                    FD_ZERO(&readers);
                    max_fd = -1;

                    FD_SET(serial_fd, &readers);
                    max_fd = Max(max_fd, serial_fd);

                    timeout.tv_sec = 0;
                    timeout.tv_usec = 500000;

                    r = select(max_fd + 1, &readers, NULL, NULL, &timeout);
                    if (r < 0) {
                        g_log("", G_LOG_LEVEL_ERROR, "error with select(): %s", strerror(errno));
                        exit(1);
                    }

                    if (connection_state == 0) {
                        if (FD_ISSET(serial_fd, &readers)) {
                            char val;
                            r = read(serial_fd, &val, 1);
                            if (r < 0) {
                                g_log("", G_LOG_LEVEL_WARNING, "error reading from serial port: %s", strerror(errno));
                                exit(1);
                            }
                            if (r == 0) {
                                g_log("", G_LOG_LEVEL_WARNING, "read on the serial port returned 0");
                                exit(1);
                            }
                            // printf("\n        Sent 0x11, received 0x%02x from CSA-CP", val);
                            if (val == 0x22) {
                                connection_state = 1;
                                num_sessions = -1;
                                printf("\n        Connection to CSA-CP ESTABLISHED\n");
                            }
                        }
                    }

                    else if (connection_state == 1 && num_sessions == -1) {
                        if (FD_ISSET(serial_fd, &readers)) {
                            char buf[100];
                            int i = 0;

                            while ((r = read(serial_fd, &buf[i], 1)) == 1) {
                                if (r < 0) {
                                    g_log("", G_LOG_LEVEL_WARNING, "error reading from serial port: %s", strerror(errno));
                                    exit(1);
                                }
                                // printf("\n\ti = %d: Sent 0x01, received 0x%02x from CSA-CP", i, buf[i]);
                                i++;
                            }
                            num_sessions = (int)buf[1];
                            printf("\n\tCSA-CP number of sessions available = %d\n", num_sessions);
                        }
                        connection_state = 2;
                    }

                }
                break;

            case 'D':
            case 'd':
                session_desired = 0;
                while (session_desired <= 0 || session_desired > num_sessions) {
                    printf("\n\tEnter session number to download: ");
                    scanf("%d", &session_desired);
                }
                printf("\nDownloading session # %d", session_desired);

                while (ready_to_download == 0) {
                    int max_fd;
                    fd_set readers;
                    int r;
                    uint8_t buf[3];

                    struct timeval timeout;

                    r = tcflush(serial_fd, TCIFLUSH);
                    if (r < 0) {
                        g_log("", G_LOG_LEVEL_WARNING, "error flushing '%s': %s", serial_port, strerror(errno));
                        close(serial_fd);
                        serial_fd = -1;
                        exit(1);
                    }

                    buf[0] = 0x82;
                    buf[1] = session_desired;
                    buf[2] = 0x02;

                    FD_ZERO(&readers);
                    max_fd = -1;

                    r = write(serial_fd, &buf, sizeof(buf));
                    if (r < 0) {
                        g_log("", G_LOG_LEVEL_WARNING, "error writing to serial port: %s", strerror(errno));
                        exit(1);
                    }
                    if (r == 0) {
                        g_log("", G_LOG_LEVEL_WARNING, "write to the serial port returned 0");
                        exit(1);
                    }

                    FD_SET(serial_fd, &readers);
                    max_fd = Max(max_fd, serial_fd);

                    timeout.tv_sec = 0;
                    timeout.tv_usec = 500000;

                    r = select(max_fd + 1, &readers, NULL, NULL, &timeout);
                    if (r < 0) {
                        g_log("", G_LOG_LEVEL_ERROR, "error with select(): %s", strerror(errno));
                        exit(1);
                    }

                    if (FD_ISSET(serial_fd, &readers)) {
                        char buf[100];
                        int i = 0;

                        while ((r = read(serial_fd, &buf[i], 1)) == 1) {
                            if (r < 0) {
                                g_log("", G_LOG_LEVEL_WARNING, "error reading from serial port: %s", strerror(errno));
                                exit(1);
                            }
                            printf("\n\ti = %d: received 0x%02x from CSA-CP", i, buf[i]);
                            i++;
                        }
                        if (buf[0] == 0x02 && buf[1] == session_desired) {
                            ready_to_download = 1;
                            printf("\n\tReady to download session = %d\n", session_desired);
                        }
                    }

                }

                // Download the desired session

                {
                    int max_fd;
                    fd_set readers;
                    int r;
                    uint8_t buf = 0x03;

                    struct timeval timeout;

                    r = tcflush(serial_fd, TCIFLUSH);
                    if (r < 0) {
                        g_log("", G_LOG_LEVEL_WARNING, "error flushing '%s': %s", serial_port, strerror(errno));
                        close(serial_fd);
                        serial_fd = -1;
                        exit(1);
                    }

                    FD_ZERO(&readers);
                    max_fd = -1;

                    r = write(serial_fd, &buf, 1);
                    if (r < 0) {
                        g_log("", G_LOG_LEVEL_WARNING, "error writing to serial port: %s", strerror(errno));
                        exit(1);
                    }
                    if (r == 0) {
                        g_log("", G_LOG_LEVEL_WARNING, "write to the serial port returned 0");
                        exit(1);
                    }

                    FD_SET(serial_fd, &readers);
                    max_fd = Max(max_fd, serial_fd);

                    timeout.tv_sec = 0;
                    timeout.tv_usec = 500000;

                    r = select(max_fd + 1, &readers, NULL, NULL, &timeout);
                    if (r < 0) {
                        g_log("", G_LOG_LEVEL_ERROR, "error with select(): %s", strerror(errno));
                        exit(1);
                    }

                    if (FD_ISSET(serial_fd, &readers)) {
                        char val;
                        r = read(serial_fd, &val, 1);
                        if (r < 0) {
                            g_log("", G_LOG_LEVEL_WARNING, "error reading from serial port: %s", strerror(errno));
                            exit(1);
                        }
                        if (r == 0) {
                            g_log("", G_LOG_LEVEL_WARNING, "read on the serial port returned 0");
                            exit(1);
                        }
                        printf("\n        Sent 0x03, received 0x%02x from CSA-CP", val);
                        if (val == 0x03) {
                            downloading = 1;
                            printf("\n        Downloading the file...\n");
                        }
                    }

                }
                
                if (downloading) {
                    int download_complete = 0;
                    int max_fd;
                    fd_set readers;
                    int r;
                    struct timeval timeout;

                    printf("\nPrint file contents...\n");
                    while (download_complete == 0) {

                        FD_ZERO(&readers);
                        max_fd = -1;

                        FD_SET(serial_fd, &readers);
                        max_fd = Max(max_fd, serial_fd);

                        timeout.tv_sec = 0;
                        timeout.tv_usec = 500000;

                        r = select(max_fd + 1, &readers, NULL, NULL, &timeout);
                        if (r < 0) {
                            g_log("", G_LOG_LEVEL_ERROR, "error with select(): %s", strerror(errno));
                            exit(1);
                        }

                        if (FD_ISSET(serial_fd, &readers)) {
                            char val;

                            while ((r = read(serial_fd, &val, 1)) == 1) {
                                if (r < 0) {
                                    g_log("", G_LOG_LEVEL_WARNING, "error reading from serial port: %s", strerror(errno));
                                    exit(1);
                                }
                                if ((int)val != -1) {
                                    // Echo the character received back
                                    r = write(serial_fd, &val, 1);
                                    if (r < 0) {
                                        g_log("", G_LOG_LEVEL_WARNING, "error writing to serial port: %s", strerror(errno));
                                        exit(1);
                                    }
                                    if (r == 0) {
                                        g_log("", G_LOG_LEVEL_WARNING, "write to the serial port returned 0");
                                        exit(1);
                                    }
                                }
                                if (val == '\r') val = '\n';
                                if (val == 0x00) {
                                    download_complete = 1;
                                    printf("\n\tDownload Complete!");
                                    fflush(NULL);
                                }
                                else {
                                    printf("%c", val);
                                }
                            }
                        }
                    }

                }
                break;

            case 'Z':
            case 'z':
                break;

            case 'Q':
            case 'q':
                break;

            default:
                break;

        }    // End switch
    
    }

    exit(0);
}

