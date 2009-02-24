
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "hardware-abstractor.h"
#include "csa-cp-hab.h"
#include "serial.h"




static bionet_node_t* node = NULL;
static bionet_resource_t *resource;


static void parse_line(char* line) {
    int i_1, i_2, i_3, i_4, i_5, i_6, i_7, i_8, i_9, i_10, i_11, i_12, i_13, i_14, i_15, i_16, i_17, i_18, i_19, i_20, i_21;
    float o2;
    int s_success;
    struct tm csa_time;

    static int base_day, base_year, base_month;
    static int serial_number, calibration_month, calibration_date, TWA_timebase, reset_month, reset_day, reset_year;


    //  not sure what TWA time base is
    //  lots of the unknowns correspond to TWA alarm, but not sure
    //      what corresponds to what
    //

    // get the maximum number of integers in the entry
    // parse the data/send it to bioned according to the number of
    // integers fscanf returns

    if (line[0] == ';') {
        //  sets the time off set

        sscanf(
            line,
            ";%d %d %d",
            &base_month,
            &base_day,
            &base_year
        );
        g_log("", G_LOG_LEVEL_DEBUG, "base_day: %d\nbase_month: %d\nbase_year: %d\n", base_day, base_month, base_year);

        return;
    }

    s_success = sscanf(
        line,
        "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", 
        &i_1, &i_2, &i_3, 
        &i_4, &i_5, &i_6, 
        &i_7, &i_8, &i_9, 
        &i_10, &i_11, &i_12, 
        &i_13, &i_14, &i_15, 
        &i_16, &i_17, &i_18, 
        &i_19, &i_20, &i_21
    );

    if (s_success == 0) {
        return;
    } else if (s_success == 6) {
        int r;
        struct timeval resource_time;

        memset(&csa_time, (char)0, sizeof(struct tm));

        // FIXME: bogus!  For the year, the csa-cp device reports the two least significant digits...
        if (base_year > 50) {
            csa_time.tm_year = base_year;
        } else {
            csa_time.tm_year = base_year + 100;
        }
        csa_time.tm_mon  = base_month - 1;
        csa_time.tm_mday = base_day;
        csa_time.tm_hour = i_1;
        csa_time.tm_min  = i_2;
        csa_time.tm_sec  = 0;

        resource_time.tv_sec = mktime(&csa_time);
        resource_time.tv_usec = 0;

        o2 = i_5 / 10.0;

        if (node == NULL) {
            node = bionet_node_new(this_hab, "csa-cp");
            if (node == NULL) {
                g_warning("error creating new Node!");
                return;
            }

            resource = bionet_resource_new(node, BIONET_RESOURCE_DATA_TYPE_FLOAT, BIONET_RESOURCE_FLAVOR_SENSOR, "O2");
            if (resource == NULL) {
                g_warning("error creating new Resource!");
                return;
            }

            r = bionet_node_add_resource(node, resource);
            if (r != 0) {
                g_warning("error adding new Resource to new Node!");
                return;
            }

            r = bionet_resource_set_float(resource, o2, NULL);
            if (r != 0) {
                g_warning("error setting O2\n");
                return;
            }

            bionet_hab_add_node(this_hab, node);

            hab_report_new_node(node);
            return;
        }

        resource = bionet_node_get_resource_by_id(node, "O2");

        r = bionet_resource_set_float(resource, o2, NULL);
        if (r != 0) {
            g_warning("error setting O2\n");
        }

        hab_report_datapoints(node);

    } else if (s_success == 21) {
        // sets the initial values
        
        serial_number = i_1;
        calibration_month = i_3;
        calibration_date = i_4;
        TWA_timebase = i_18;
        reset_day = i_19;
        reset_month = i_20;
        reset_year = i_21;
    }
}




int download_session(serial_handle_t serial_handle, int session_number, int record_raw_data) {
    char init_download[4];
    char line[1000];
    char received[3];
    int r, i = 0, j = 0, k = 0, num_chars = 0;

    FILE* fp = NULL;


    init_download[0] = 0x82;
    init_download[1] = 0x00;
    init_download[2] = 0x02;
    init_download[3] = 0x03;


    //  
    //  Clean the ports
    //

    r = serial_flush(serial_handle);
    if (r < 0) {
        return -1;
    }


    // 
    // optionally open a file to record the raw data from the CSA-CP
    // FIXME: we should have a better way to specify the filename
    //

    if (record_raw_data) {
        time_t time_sec;
        struct tm *computer_time;

        time_sec = time(NULL);
        computer_time = localtime(&time_sec);
        if (computer_time == NULL) {
            g_log("", G_LOG_LEVEL_ERROR, "problem using gettimeofday(): %s", strerror(errno));
        } else {
            char time_str[100];
            if (strftime(time_str, 100, "%Y-%m-%d_%H.%M.%S", computer_time) <= 0) {
                g_log("", G_LOG_LEVEL_ERROR, "problem with strftime");
            } else {
                char file_name[1000];
                int r;
                r = snprintf(file_name, sizeof(file_name), "CSA-CP.%d.%s", (i+1), time_str);
                if (r >= sizeof(file_name)) {
                    g_log("", G_LOG_LEVEL_ERROR, "filename too long, skipping raw data file!");
                } else {
                    fp = fopen(file_name, "a");
                    if (fp == NULL) {
                        g_log("", G_LOG_LEVEL_ERROR, "unable to open write file %s", file_name);
                    }
                }
            }
        }
    }


    // 
    //  First, we initialize the download
    //

    k = 0;
    while (k < session_number) {
        init_download[1] ++;

        r = serial_write(serial_handle, init_download, 3);
        if (r < 3) {
            g_log("", G_LOG_LEVEL_DEBUG, "download_session(): couldn't write command");
            goto fail;
        }

        j = 0;
        while (j < 20) {
            r = serial_read(serial_handle, &received[i], 2, 100*1000);
            if (r == 1) {
                // printf("    download_session(): recieved_char = %02hhx", received[i]);
                i++;
            }
            if (i >= 1) {
                break;
            }
            g_usleep(2000);
            j++;
        }

        if ((received[0] != '\x02') && ((int)received[1] != k)) {
            // printf("bad handshake\n");
            goto fail;
        }
        
        // printf("finished first handshake\n");
        k++;
    }

    //  One last handshake, this time sending '\x03'

    r = serial_write_byte(serial_handle, init_download[3]);
    if (r < 1) {
        goto fail;
    }

    r = serial_read(serial_handle, &received[0], 1, 500*1000);
    if (r < 1) {
        goto fail;
    }

    if (received[0] != '\x03') {
        g_log("", G_LOG_LEVEL_WARNING, "problem reading from CSA-CP, aborting download");
        goto fail;
    }
    
    // printf("finished second handshake\n");
    
    while(received[0] != '\x00') {
        char buf;

        r = serial_read(serial_handle, &buf, 1, 500*1000);
        if (r < 1) {
            g_warning("problem downloading (might have been disconnected)");
            goto fail;
        }

        received[0] = buf;
        r = serial_write_byte(serial_handle, buf);
        if (r < 0) {
            goto fail;
        }

        if ((buf == '\n') || (buf == '\r')) {
            line[num_chars] = '\n';
            if (fp) fprintf(fp, "%c", '\n');
        } else {
            line[num_chars] = buf;
            if (fp) fprintf(fp, "%c", buf);
        }
        num_chars++;

        if (num_chars >= 999) {
            g_log("", G_LOG_LEVEL_WARNING, "line too large, cannot parse");
            goto fail;
        }
        if ((line[num_chars-1] == '\n') || (line[num_chars-1] == '\r')) {
            line[num_chars] = '\0';
            parse_line(line);
            num_chars = 0;
        }
    }

    g_debug("finished downloading session");

    bionet_hab_remove_node_by_id(this_hab, bionet_node_get_id(node));
    
    hab_report_lost_node("csa-cp");

    bionet_node_free(node);
    node = NULL;

    if (fp) fclose(fp);

    return 0;


fail:
    if (node != NULL) {
        hab_report_lost_node("csa-cp");
        bionet_node_free(node);
        node = NULL;
    }

    if (fp) fclose(fp);
    return -1;
}

