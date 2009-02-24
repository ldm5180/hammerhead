
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <glib.h>
#include <unistd.h>


#include "cpod.h"

struct params_info_request
{
    uint8_t messages_per_second;
    uint8_t sampling_period;
    uint8_t num_samples;
    uint8_t offset;
} __attribute__ ((packed));

struct sampling_params_command
{
    uint8_t frame_marker;
    uint8_t size;
    uint8_t cmd;
    struct params_info_request opcodes[NUM_OPCODES];
    uint8_t sequence_num;
    uint16_t crc;
} __attribute__ ((packed));

struct sampling_params_response
{
    uint8_t frame_marker;
    uint8_t size;
    uint8_t cmd;
    uint8_t sequence_num;
    uint16_t crc;
} __attribute__ ((packed));


int sampling_parameters(int fd)
{
    int r;
    struct sampling_params_command command;
    struct sampling_params_response response;

    command.frame_marker = FRAME_MARKER;
    command.size         = sizeof(command) - 4;
    command.cmd          = SAMPLING_PARAMETERS << CMD_OFFSET;
    command.sequence_num = next_command_number();

    // ECG - Lead II
    command.opcodes[0].messages_per_second = 0x08;
    command.opcodes[0].sampling_period = 0x01;
    command.opcodes[0].num_samples = 0x20;
    command.opcodes[0].offset = 0x00;

    // ECG - Lead V5
    command.opcodes[1].messages_per_second = 0x08;
    command.opcodes[1].sampling_period = 0x01;
    command.opcodes[1].num_samples = 0x20;
    command.opcodes[1].offset = 0x00;

    // Respiration Raw
    command.opcodes[2].messages_per_second = 0x01;
    command.opcodes[2].sampling_period = 0x20;
    command.opcodes[2].num_samples = 0x30;
    command.opcodes[2].offset = 0xff;

    // Acceleration X
    command.opcodes[3].messages_per_second = 0x02;
    command.opcodes[3].sampling_period = 0x01;
    command.opcodes[3].num_samples = 0x20;
    command.opcodes[3].offset = 0xff;

    // Acceleration Y
    command.opcodes[4].messages_per_second = 0x08;
    command.opcodes[4].sampling_period = 0x01;
    command.opcodes[4].num_samples = 0x20;
    command.opcodes[4].offset = 0xff;

    // Acceleration Z
    command.opcodes[5].messages_per_second = 0x08;
    command.opcodes[5].sampling_period = 0x01;
    command.opcodes[5].num_samples = 0x20;
    command.opcodes[5].offset = 0xff;

    // Skin Temperature
    command.opcodes[6].messages_per_second = 0x08;
    command.opcodes[6].sampling_period = 0x01;
    command.opcodes[6].num_samples = 0x20;
    command.opcodes[6].offset = 0xff;

    // Pulse Oximetry
    command.opcodes[7].messages_per_second = 0x08;
    command.opcodes[7].sampling_period = 0x01;
    command.opcodes[7].num_samples = 0x20;
    command.opcodes[7].offset = 0xff;

    // Heart Rate
    command.opcodes[8].messages_per_second = 0x08;
    command.opcodes[8].sampling_period = 0x01;
    command.opcodes[8].num_samples = 0x20;
    command.opcodes[8].offset = 0xff;

    command.crc = g_htons(crcccitt((char*)&command.cmd, command.size));

    r = reliable_write(fd, &command, sizeof(command));

    if (r != sizeof(command))
        return -1;

    g_usleep(5*1000*1000);

    r = patient_read(fd, &response, sizeof(response));

    g_debug("r = %d", r);

    if (r == -1 ||
        verify_response((char*)&command, (char*)&response))
        return -2;

    return 0;
}
