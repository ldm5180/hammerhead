
//
// Copyright (C) 2008-2009, Regents of the University of Colorado.
//

#include <errno.h>
#include <glib.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>


#include "amedevice.h"
#include "hardware-abstractor.h"

// Audio calculations
#define CHANNELS 1
#define FRAME_SIZE 240
#define SAMPLE_PADDING 8  // Equal to 9600/1200


extern uint32_t gain;

extern bionet_node_t *node;

int16_t mulaw2pcm16(uint8_t byte);


int unbyte_escape_and_copy(char* dst, char* src, int size)
{
    static int last_was_escape = 0;
    int i, j;

    if (last_was_escape &&
        src[0] == (char)(0xff))
    {
        last_was_escape = 0;
        i = 1;
    }
    else
        i = 0;

    for (j = 0; i < size; i++, j++)
    {
        if (src[i] == (char)(0xff))
        {
            if (i+1 >= size)
                last_was_escape = 1;
            else if (src[i+1] == (char)(0xff))
                i++;
        }

        memcpy(&dst[i], &src[j], 1);
    }

    return j;
}


int read_data_from_stethoscope_and_write(int fd, bionet_stream_t * stream, int num_listeners)
{
    static char buffer[512];
    static int  buffer_size = 0;
    static int  state = RSP_STATUS_WAITING;

    int r;

    bionet_resource_t * resource;

    if (state == RSP_STATUS_WAITING)
    {
        r = read(fd, buffer+buffer_size, 8-buffer_size);

        if (r <= 0)
        {
            if (errno == EAGAIN)
                return 0;

            return -1;
        }

        buffer_size += r;

        if (buffer_size == 8)
        {
            if ((r = ame_is_modem_command(buffer)))
            {
                r = read(fd, buffer+buffer_size, r-buffer_size);
                buffer_size = 0;
            }
            else
                state = RSP_STATUS_GOT_COMMAND;
        }
    }

    if (state == RSP_STATUS_GOT_COMMAND)
    {
        switch ((uint8_t)buffer[1])
        {
        case '#':
            g_debug("attempting to purge buffer of crap...buffer_size = %d", buffer_size);
            memcpy(buffer, buffer+6, buffer_size - 6);
            buffer_size -= 6;
            state = RSP_STATUS_WAITING;
            break;

        case RSP_HEADER:
            g_debug("attempting to purge buffer of crap byte...buffer_size = %d", buffer_size);
            memcpy(buffer, buffer+1, buffer_size - 1);
            buffer_size -= 1;
            state = RSP_STATUS_WAITING;
            break;

        case CMD_READ_BATTERY_STATUS:
            g_debug("Received battery response");
            {
                float voltage;
                struct timeval tv;

                r = gettimeofday(&tv, NULL);
                if (r < 0) {
                    g_warning("error with gettimeofday: %s", strerror(errno));
                    return -1;
                }

                r = read(fd, buffer+buffer_size, 12-buffer_size);

                if (r <= 0)
                {
                    if (errno == EAGAIN)
                        return 0;

                    return -1;
                }

                buffer_size += r;

                voltage = ame_parse_battery_voltage(buffer, buffer_size);
                g_debug("Got the battery voltage: %f, size = %d", voltage, buffer_size);

		resource = bionet_node_get_resource_by_id(node, "Battery-Voltage");
                r = bionet_resource_set_float(resource, voltage, &tv);

                if (r < 0)
                    g_warning("error updating resource value!");
                else
                    hab_report_datapoints(node);
            }

            buffer_size = 0;
            state = RSP_STATUS_WAITING;
            
            break;

        case CMD_RETURN_DATA:
        {
            // Need to byte escape the bytes that we've already read in...
            char tmp[5] = {buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]};

            buffer_size = 3;
            buffer_size += unbyte_escape_and_copy(tmp, buffer+buffer_size, 5);

            state = RSP_STATUS_SAMPLING_ENABLED;
        }
            break;

        default:
            g_debug("Unknown response: %c - Clearing and starting over", buffer[1]);
            buffer_size = 0;
            state = RSP_STATUS_WAITING;
            break;
        }
    }

    if ((state == RSP_STATUS_SAMPLING_ENABLED) && (num_listeners))
    {
        static int size = 248;

        char tmp[256];

        r = read(fd, tmp, size-buffer_size);

        if (r <= 0)
        {
            if (errno == EAGAIN)
                return 0;

            return -1;
        }

        buffer_size += unbyte_escape_and_copy(buffer+buffer_size, tmp, r);

        if (buffer_size == size)
        {
            uint16_t frame = 0;
            int16_t pcm16data[SAMPLE_PADDING*FRAME_SIZE*CHANNELS];
            int i = 0;


            frame += (uint8_t)buffer[4];
            frame += (uint8_t)(buffer[3] << 8);
            g_debug("Got streaming frame %d, start bytes are %c %c %c 0x00 %c %c %c %c", frame,
                    buffer[0], buffer[1], buffer[2], /*buffer[3],*/ buffer[4], buffer[5], buffer[6], buffer[7]);


            // Upsample for minimum alsa rate and for stereo
            // This method sounds like total shit!!!  MUST LPF!
            // Also, because 22050/1200 isn't an integer, output will
            // underrun about every 5 seconds.  Total shit!!
            for (i = 0; i < FRAME_SIZE; i++)
            {
                int k;

                for (k = 0; k < SAMPLE_PADDING*CHANNELS; k++)
                    pcm16data[SAMPLE_PADDING*CHANNELS*i+k] = mulaw2pcm16((uint8_t)buffer[i+5]) * gain;
            }


	    hab_publish_stream(stream, pcm16data, sizeof(pcm16data));

            buffer_size = 0;
            state = RSP_STATUS_WAITING;
        }
    }


    return 0;
}
