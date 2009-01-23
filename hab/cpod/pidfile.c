
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#define PIDFILE_NAME "/tmp/cpod.pid"


void cleanup_pidfile()
{
    g_log("", G_LOG_LEVEL_INFO, "cpod quitting...");
    unlink(PIDFILE_NAME);
    exit(0);
}

int kill_kin()
{
    int pid_fd;
    char buffer[10];

    ssize_t r;
    pid_t pid;


    pid_fd = open(PIDFILE_NAME, O_RDONLY);

    if (pid_fd < 0)
    {
        if (errno == ENOENT)
            return 0;
        else
            return -1;
    }


    r = read(pid_fd, buffer, sizeof(buffer));
    r = sscanf(buffer, "%d", &pid);
    close(pid_fd);

    if (r != 1)
    {
        unlink(PIDFILE_NAME);
        return -2;
    }

    r = kill(pid, SIGINT);

    if (r != 0)
    {
        if (errno == ESRCH)
        {
            unlink(PIDFILE_NAME);
            return 0;
        }
        else
            return -3;
    }

    return 0;
}


int write_pidfile()
{
    int pid_fd, len, r;
    char str[10];

    struct sigaction sa;


    kill_kin();

    pid_fd = open(PIDFILE_NAME, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (pid_fd < 0)
        return -1;

    len = snprintf(str, sizeof(str), "%d\n", getpid());

    r = write(pid_fd, str, len);

    close(pid_fd);

    if (r != len)
        return -2;


    //atexit(cleanup_pidfile); 

    // handle exit signals
    sa.sa_handler = cleanup_pidfile;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &sa, NULL) < 0)
        return -3;

    if (sigaction(SIGTERM, &sa, NULL) < 0)
        return -4;

    return 0;
}

