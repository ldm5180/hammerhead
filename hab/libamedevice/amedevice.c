
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <bluetooth/bluetooth.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include <glib.h>


#include "amedevice.h"


int ame_write_command(int fd, char* buffer, int size)
{
    char cmd[CMD_SIZE];

    //  AME commands are CMD_BYTES long, so pad with 0's before writing
    //  checksum to last byte, then send
    memset(cmd, 0, CMD_SIZE);
    memcpy(cmd, buffer, size);

    cmd[CMD_SIZE - 1] = sum8(buffer, size) & SUM8_MASK;

    return write(fd, cmd, CMD_SIZE);
}


int ame_read_command(int fd, char* buffer, int size)
{
    int i;
    char tmp[256];


    i = 0;


    if (size < 8)
        goto end;


start:
    // Start with the first 8 bytes, as there could be a bunch of shit in the buffer

    // Timeout if there is nothing on the other end
    {
        int r;
        fd_set reader;
        struct timeval tv = {1,0};

        FD_ZERO(&reader);
        FD_SET(fd, &reader);

        r = select(fd+1, &reader, NULL, NULL, &tv);

        if (r < 0)
            return -1;

        if (r == 0)
            return 0;
    }
    i = read(fd, tmp, 8);


    if (tmp[0] != ((char)RSP_HEADER))
    {
        // On startup, the AME devices return wierd modem commands - dump on floor
        if (tmp[0] == 'A' &&
            tmp[1] == 'T')
        {
            if (tmp[6] == 'B')
            {
                i += read(fd, tmp+i, 14 - i);
                goto start;
            }
            else if (tmp[6] == 'S')
            {
                i += read(fd, tmp+i, 21 - i);
                goto start;
            }
            else if (tmp[6] == 'E')
            {
                i += read(fd, tmp+i, 36 - i);
                goto start;
            }
            else if (tmp[6] == 'L')
            {
                i += read(fd, tmp+i, 38 - i);
                goto start;
            }
            else if (tmp[6] == 'U')
            {
                i += read(fd, tmp+i, 35 - i);
                goto start;
            }
            else return -3;
            
        }
        //else return -2;
    }

end:
    i += read(fd, tmp+i, size-i);

    memcpy(buffer, tmp, i);

    return i;
}


int ame_is_modem_command(char* buffer)
{
    if (buffer[0] == 'A' &&
        buffer[1] == 'T')
    {
        if (buffer[6] == 'B')
            return 14;

        else if (buffer[6] == 'S')
            return 21;

        else if (buffer[6] == 'E')
            return 36;

        else if (buffer[6] == 'L')
            return 38;

        else if (buffer[6] == 'U')
            return 35;
    }

    return 0;
}


int ame_command_return_status(int fd)
{
    char cmd[2] = {CMD_RETURN_STATUS, 0x01};

    return ame_write_command(fd, cmd, sizeof(cmd));
}


int ame_command_load_config(int fd)
{
    char cmd[7] = {CMD_LOAD_CONFIGURATION, 0x01};

    //  Need some more trickery here for other bytes...

    return ame_write_command(fd, cmd, sizeof(cmd));
}


int ame_command_return_config(int fd)
{
    char cmd[2] = {CMD_RETURN_CONFIGURATION, 0x01};

    return ame_write_command(fd, cmd, sizeof(cmd));
}


int ame_command_return_data(int fd, uint16_t sample_rate, uint8_t gain)
{
    char cmd[7] = {CMD_RETURN_DATA, 0x01};

    uint16_t timer_offset;

    if (gain > 99)
        return -1;

    timer_offset = (uint16_t)(65536.0 - ((1.0 / sample_rate) / 0.542535e-6)) + DEVICE_DELAY;  // ??
    timer_offset = g_htons(timer_offset);
    memcpy(&cmd[2], &timer_offset, sizeof(timer_offset));

    memcpy(&cmd[4], &sample_rate, sizeof(sample_rate));

    cmd[6] = gain;

    return ame_write_command(fd, cmd, sizeof(cmd));
}


int ame_command_return_error(int fd)
{
    char cmd[2] = {CMD_RETURN_ERROR_INFO, 0x01};

    return ame_write_command(fd, cmd, sizeof(cmd));
}


int ame_command_return_firmware_revision(int fd)
{
    char cmd[2] = {CMD_RETURN_FIRMWARE_RELEASE, 0x01};

    return ame_write_command(fd, cmd, sizeof(cmd));
}


int ame_command_load_bdaddr(int fd, bdaddr_t* addr, char* pin)
{
    char cmd[24] = {CMD_LOAD_BLUETOOTH_ADDRESS, 0x01};

    //  Load the bdaddr and pin into cmd...

    return ame_write_command(fd, cmd, sizeof(cmd));
}


int ame_command_return_bdaddr(int fd)
{
    char cmd[2] = {CMD_RETURN_BLUETOOTH_ADDRESS, 0x01};

    return ame_write_command(fd, cmd, sizeof(cmd));
}


int ame_command_reset_error(int fd)
{
    char cmd[2] = {CMD_RESET_ERROR_INFO, 0x01};

    return ame_write_command(fd, cmd, sizeof(cmd));
}


int ame_command_load_name(int fd)
{
    char cmd[22] = {CMD_LOAD_NAME, 0x01};

    // Load the name into cmd...

    return ame_write_command(fd, cmd, sizeof(cmd));
}


int ame_command_return_name(int fd)
{
    char cmd[2] = {CMD_RETURN_NAME, 0x01};

    return ame_write_command(fd, cmd, sizeof(cmd));
}


char* ame_response_return_name(int fd)
{
    int i;
    char *name;


    name = malloc(25);
    i = ame_read_command(fd, name, 24);
    name[24] = '\0';


    if (name[1] == ((char)CMD_RETURN_NAME) &&
        name[2] == ((char)RSP_STATUS_SUCCESS) &&
	(name <= (char *)UINTPTR_MAX-3))
        return name+3;


    free(name);
    return NULL;

}


int ame_command_erase(int fd)
{
    char cmd[6] = {CMD_ERASE, 0x01, 0xDE, 0xAD, 0xBE, 0xEF};  // Dead beef??

    return ame_write_command(fd, cmd, sizeof(cmd));
}


int ame_command_return_type(int fd)
{
    char cmd[2] = {CMD_RETURN_DEVICE_TYPE, 0x01};

    return ame_write_command(fd, cmd, sizeof(cmd));
}


int ame_command_close_connection(int fd)
{
    char cmd[2] = {CMD_CLOSE_CONNECTION, 0x01};

    return ame_write_command(fd, cmd, sizeof(cmd));
}


int ame_command_return_battery_status(int fd)
{
    char cmd[2] = {CMD_READ_BATTERY_STATUS, 0x01};

    return ame_write_command(fd, cmd, sizeof(cmd));
}


float ame_parse_battery_voltage(char* buffer, int size)
{
    float voltage;


    if (size != 12)
        return -1.0;

    voltage = (float)(g_ntohs(*((uint16_t*)&buffer[3])));

    //  Experimentally determined factor
    voltage = voltage * 0.00648276;
    
    return voltage;
}


int ame_command_keep_alive(int fd, uint16_t timeout)
{
    char cmd[4] = {CMD_READ_BATTERY_STATUS, 0x01};

    uint16_t timeout_nbo;

    timeout_nbo = g_htons(timeout);
    memcpy(&cmd[2], &timeout_nbo, sizeof(timeout_nbo));

    return ame_write_command(fd, cmd, sizeof(cmd));
}
