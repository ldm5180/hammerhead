
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <string.h>


#ifdef _WIN32
    #include <winsock.h>
#endif




int bionet_get_network_error(void) {

#if defined(LINUX) || defined(MACOSX)
    return errno;
#endif

#ifdef _WIN32
    int error = WSAGetLastError();

    if (error == WSAEWOULDBLOCK)
      return EAGAIN;
    
    return error;
#endif

}




const char *bionet_get_network_error_string(void) {

#if defined(LINUX) || defined(MACOSX)
    return strerror(errno);
#endif

#ifdef _WIN32
    static char error_message[256];

    sprintf(error_message, "error %d", WSAGetLastError());
    return (const char *)error_message;
#endif

}

