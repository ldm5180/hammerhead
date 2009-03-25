
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef CPOD_H
#define CPOD_H

#include <stdint.h>
#include "hardware-abstractor.h"

//  Ugly hack for the demo
#define CPOD_BDADDR "00:A0:96:20:8A:C1"
#define NODE_ID     "00-A0-96-20-8A-C1"

#define HAB_TYPE "CPOD"
#define DEFAULT_NAG "bionet-basestation.colorado.edu"



//  Command Codes

#define NO_OPERATION 0x0
#define START_DOWNLOAD 0x01
#define START_STREAMING 0x02
#define END_SESSION 0x03
#define AVAILABLE_OPCODES 0x04
#define SAMPLING_PARAMETERS 0x05
#define NEXT_PACKET_DOWNLOAD 0x06
#define NEXT_PACKET_STREAMING 0x07
#define NEXT_PACKET_LOGGING 0x08
#define SET_TIME 0x09
#define RESET 0x0A
#define STATUS 0x0B
#define HANDSHAKE 0x0C
#define SIM 0x0D
//  0x0E NOT_USED
#define READ_TIMER 0X0F


#define CMD_OFFSET 4

#define NUM_OPCODES 9


//  Message definitions

#define SYNC 0x00
#define FRAME_MARKER 0xFF

uint8_t next_command_number();

void init_crcccitt_table();

int create_zeroed_float_sensor(bionet_node_t* node, char* id);

void set_float_resource(bionet_node_t* node, char *id, float value, struct timeval *tv);

uint16_t crcccitt(char* data, int size);

int verify_response(char* command, char* response);

#include <sys/select.h>
#include <sys/types.h>
size_t reliable_write(int fd, const void* buffer, size_t size);

size_t patient_read(int fd, void *buffer, size_t count);

//  CPOD protocol functions
//  All return 0 on success, and ! 0 on failure

int acknowledge_command( int fd );

int start_download( int fd );

int start_streaming( int fd );

int end_session( int fd );

int available_opcodes ( int fd );

int sampling_parameters( int fd );

int send_next_packet_download( int fd );

int send_next_packet_streaming( int fd );

int read_next_packet( int fd );

int set_time(int fd);

int send_reset( int fd );

int send_status( int fd );

int shake_hands(int fd);

void reset_bookkeeping( );

#endif
