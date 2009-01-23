
//
// Copyright (C) 2004-2008, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of GNU General Public License version 2, as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//


#include <errno.h>
#include <glib.h>
#include <string.h>
#include <unistd.h>


#include "cpod.h"
#include "hardware-abstractor.h"


struct data_header
{
    uint8_t frame_header;
    uint8_t size;
    uint8_t command;
    uint8_t junk;
    uint8_t flags;
} __attribute__ ((packed));


struct cpod_data
{
    uint8_t ecg_II_data[48];
    uint8_t ecg_V5_data[48];
    uint8_t respiration_data[12];
    uint8_t accel_x_data[3];
    uint8_t accel_y_data[3];
    uint8_t accel_z_data[3];
    uint8_t temp_data[2];
    uint8_t sp_O2_data[2];
    uint8_t heart_rate[2];
}__attribute__ ((packed));


inline int is_blood_pressure(uint8_t flags)
{
    return flags & 0x08;
}

inline int is_gps_data(uint8_t flags)
{
    return flags & 0x10;
}

inline int is_co2_data(uint8_t flags)
{
    return flags & 0x20;
}

inline int is_event(uint8_t flags)
{
    return flags & 0x01;
}

inline int lost_data(uint8_t flags)
{
    return flags & 0x02;
}

inline uint16_t first_group(uint16_t data)
{
    return g_ntohs(data) >> 4;
}

inline uint16_t second_group(uint16_t data)
{
    return g_ntohs(data) & 0x0fff;
}

struct timeval subtract_time(struct timeval tv, suseconds_t u_diff)
{
    if (u_diff > tv.tv_usec)
    {
        tv.tv_sec -= 1;
        tv.tv_usec = tv.tv_usec+1000000-u_diff;
    }
    else
        tv.tv_usec -= u_diff;

    return tv;
}

struct timeval add_time(struct timeval tv, suseconds_t u_diff)
{
    if ((u_diff + tv.tv_usec) >= 1000000)
    {
        tv.tv_usec = (tv.tv_usec + u_diff)-1000000;
        tv.tv_sec += 1;
    }
    else
        tv.tv_usec += u_diff;

    return tv;
}

extern bionet_node_t *node;

extern int print_ecg;


int read_next_packet(int fd)
{
    static int middle_of_packet = 0;
    static unsigned char count = 0;
    static unsigned char buffer[256];

    int r, size;


    if (!middle_of_packet)
    {
        r = read(fd, buffer, 2);

        if (r == -1)
        {
            g_critical("Read failed!: %s", strerror(errno));
            return -1;
        }

        if (r == 1)
        {
            while(read(fd, buffer+1, 1) != 1);
        }

        count = 2;
        middle_of_packet = 1;

        if (buffer[0] != 0xff ||
            (buffer[1] != 0x7f && buffer[1] != 0x80))
        {
            int i;

            g_debug("WTF, mate? not 0xff.  It's %x, size = %d", (int)buffer[0], (int)buffer[1]);

            for(i = 0;i < 500; i++) {
                int r;
                r = read(fd, buffer, 256);
                if (r < 0) {
                    g_critical("Read failed!: %s", strerror(errno));
                    return -1;
                }
            }

            count = 0;
            middle_of_packet = 0;
        }

        return 0;
    }

    size = buffer[1];

    r = read(fd, buffer+count, (size+4)-count);

    /*g_debug("Read some bytes: r = %d, count was = %d, size = %d, errno = %s",
      r, count, size, strerror(errno));*/

    if (r == -1)
    {
        if(errno == EAGAIN)
            return 0;

        g_critical("Read failed!: %s", strerror(errno));
        return -1;
    }

    count += r;

    if (count == size+4)
    {
        int flag_data;
        struct data_header *header;

        flag_data = 0;

        g_debug("Got packet %d, count = %d!!", buffer[size+1], count);

        // Parse the packet
        
        if(verify_response(NULL, (char*)buffer))
            g_critical("Packet isn't legit!!");


        header = (struct data_header*)buffer;

        if (is_event(header->flags))
        {
            g_message("CPOD Event");
        }
        if (lost_data(header->flags))
        {
            flag_data += 1;
            g_message("Lost %d frames", buffer[5]);
        }
        if (is_blood_pressure(header->flags))
        {
            flag_data += 2;
            g_message("BP data");
        }
        if (is_gps_data(header->flags))
        {
            flag_data += 64;
            g_message("GPS data");
        }
        if (is_co2_data(header->flags))
        {
            flag_data += 40;
            g_message("CO2 data");
        }


        {
            struct cpod_data *data;
            struct timeval tv, tmp;

            r = gettimeofday(&tv, NULL);
            if (r < 0) {
                g_warning("error with gettimeofday: %s", strerror(errno));
                g_usleep(100*1000);
                return 0;
            }

            data = (struct cpod_data *)(buffer + sizeof(struct data_header) + flag_data);

            uint16_t accel_x, accel_y, accel_z;
            uint16_t ecg_II[16], ecg_V5[16];
            float Accel_x, Accel_y, Accel_z;
            float ECG_II[16], ECG_V5[16];

            //  Accel data: Only pull the first sample
            {
                memcpy((void*)&accel_x, data->accel_x_data, 2);
                memcpy((void*)&accel_y, data->accel_y_data, 2);
                memcpy((void*)&accel_z, data->accel_z_data, 2);

                accel_x = first_group(accel_x);
                accel_y = first_group(accel_y);
                accel_z = first_group(accel_z);

                Accel_x = ((float)accel_x - 1046.5)*(0.0123313) - 12.40;
                Accel_y = ((float)accel_y - 1046.5)*(0.0126358) - 12.63;
                Accel_z = ((float)accel_z - 1059)*(0.01236979) - 13.18;

                set_float_resource(node, "Accel-X", Accel_x, &tv);
                set_float_resource(node, "Accel-Y", Accel_y, &tv);
                set_float_resource(node, "Accel-Z", Accel_z, &tv);
            }

            // ECG data
            if (print_ecg)
            {
                int i;

                tmp = subtract_time(tv, 125000);

                for(i = 0; i < 8; i++)
                {
                    memcpy((void*)&ecg_II[i*2], &data->ecg_II_data[i*2], 2);
                    memcpy((void*)&ecg_II[i*2+1], &data->ecg_II_data[i*2+1], 2);
                    memcpy((void*)&ecg_V5[i*2], &data->ecg_V5_data[i*2], 2);
                    memcpy((void*)&ecg_V5[i*2+1], &data->ecg_V5_data[i*2+1], 2);

                    ECG_II[i*2] = (float)first_group(ecg_II[i*2])/4096.0;
                    ECG_II[i*2+1] = (float)first_group(ecg_II[i*2+1])/4096.0;
                    ECG_V5[i*2] = (float)first_group(ecg_V5[i*2])/4096.0;
                    ECG_V5[i*2+1] = (float)first_group(ecg_V5[i*2+1])/4096.0;

                    set_float_resource(node, "ECG-II", ECG_II[i*2], &tmp);
                    set_float_resource(node, "ECG-V5", ECG_V5[i*2], &tmp);

                    hab_report_datapoints(node);

                    tmp = add_time(tmp, 3910);
                    set_float_resource(node, "ECG-II", ECG_II[i*2+1], &tmp);
                    set_float_resource(node, "ECG-V5", ECG_V5[i*2+1], &tmp);

                    hab_report_datapoints(node);
                    tmp = add_time(tmp, 3909);
                }
            }

            // Sp O2 data
            {
                float Sp_O2;
                uint16_t Sp_O2_data;

                memcpy(&Sp_O2_data, data->sp_O2_data, 2);
                Sp_O2_data = g_ntohs(Sp_O2_data);

                Sp_O2 = (float)Sp_O2_data*0.0625;

                if (Sp_O2 < 102.0)
                    set_float_resource(node, "Pulse-Oximetry", Sp_O2, &tv);
            }

            // Temperature data
            {
                float Temp;
                uint16_t Temp_data;

                memcpy(&Temp_data, data->temp_data, 2);
                Temp_data = g_ntohs(Temp_data);

                Temp = (float)Temp_data*0.0045 - 48.68;
                set_float_resource(node, "Temperature", Temp, &tv);
            }

            // Heart Rate
            {
                float Heart;
                uint16_t Heart_data;

                memcpy(&Heart_data, data->heart_rate, 2);
                Heart_data = g_ntohs(Heart_data);

                Heart = (float)Heart_data*0.0625;
                if(Heart < 200.0)
                    set_float_resource(node, "Heart-Rate", Heart, &tv);
            }

            hab_report_datapoints(node);
        }

        count = 0;
        middle_of_packet = 0;
        return 1;
    }


    return 0;
}

