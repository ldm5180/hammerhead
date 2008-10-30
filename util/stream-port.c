
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bionet-util.h"


const char *bionet_stream_port_to_string(uint16_t port) {
    static char port_str[6];

    snprintf(port_str, 6, "%hu", port);
    return port_str;
}


uint16_t bionet_stream_port_from_string(const char *port_string) {
    unsigned long val;
    char *end;


    val = strtoul(port_string, &end, 0);
    if ((*port_string == '\0') || (*end != '\0')) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_port_from_string(): invalid port passed in: '%s'", port_string);
        return 0;
    }
        
    if (val > UINT16_MAX) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_port_from_string(): invalid port passed in: '%s'", port_string);
        return 0;
    }

    return (uint16_t)val;
}

