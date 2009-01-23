
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <glib.h>
#include <inttypes.h>


#include "cpod.h"

struct command_header
{
    uint8_t frame_marker;
    uint8_t size;
    uint8_t command;
    
} __attribute__ ((packed));

int verify_response(char* command, char* response)
{
    struct command_header *rsp_header;
    //uint16_t checksum;

    //g_debug("Verifying response...");

    rsp_header = (struct command_header*)response;
    
    /*rsp_header.frame_marker = (uint8_t)response[0];
      rsp_header.size = (uint8_t)response[1];
    rsp_header->command = (char)rsp_header->command << CMD_OFFSET;*/

    if (rsp_header->frame_marker != 0xff)
      return -1;

    /*if (rsp_header->command != command[2])
        return -2;

    checksum = g_htons(crcccitt((char*)response+2, rsp_header.size));
    if (checksum != *(uint16_t*)(response+2+header.size))
        return -3;*/
    
    return 0;
}
