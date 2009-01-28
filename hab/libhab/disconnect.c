
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(LINUX) || defined(MACOSX)
    #include <netdb.h>
    #include <pwd.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#include <glib.h>

#include "cal-server.h"

#include "libhab-internal.h"
#include "hardware-abstractor.h"


void hab_disconnect(void) {
    cal_server.shutdown();
    libhab_cal_fd = -1;
}
