
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <stdio.h>
#include <string.h>


#ifdef WINDOWS
    #include <winsock.h>
#endif




int bionet_get_network_error(void) {

#if defined(LINUX) || defined(MAC_OSX)
    return errno;
#endif

#ifdef WINDOWS
    int error = WSAGetLastError();

    if (error == WSAEWOULDBLOCK)
      return EAGAIN;
    
    return error;
#endif

}




const char *bionet_get_network_error_string(void) {

#if defined(LINUX) || defined(MAC_OSX)
    return strerror(errno);
#endif

#ifdef WINDOWS
    static char error_message[256];

    sprintf(error_message, "error %d", WSAGetLastError());
    return (const char *)error_message;
#endif

}

