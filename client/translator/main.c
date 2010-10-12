#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>

#include "translator.h"

#define HAB_TYPE "translator"

int main(int argc, char* argv[])
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    bionet_hab_t *hab;

    char *hab_type = HAB_TYPE;
    char *hab_id = NULL;

    int bionet_fd, bionet_hab_fd, i; 

    while(1)
    {
        int c;
        static struct option long_options[] =
        {
            {"help", 0, 0, '?'},
            {"version", 0, 0, 'v'},
            {"id", 1, 0, 'i'},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "?hvi:", long_options, &i);
        if(c == -1)
        {
            break;
        }

        switch (c)
        {
            case '?':
            case 'h':
                usage();
                return 0;

            case 'i':
                hab_id = optarg;
                break;

            case 'v':
                print_bionet_version(stdout);
                return 0;

            default:
                break;
        }
    }

    // read ini file
    int r = translator_read_ini("translator.ini");
    if(0 != r)
    {
        printf("failed to read ini file.\n");
        return 1;
    }

    // connect to bionet
    bionet_fd = bionet_connect();
    if(bionet_fd < 0)
    {
        fprintf(stderr, "Error connecting to Bionet, exiting.\n");
        return 1;
    }
    
    // connect hab to bionet
    hab = bionet_hab_new(hab_type, hab_id);
    bionet_hab_fd = hab_connect(hab);
    if(bionet_hab_fd < 0)
    {
        fprintf(stderr, "Error connecting hab to Bionet, exiting.\n");
        return 1;
    }

    // initialize all calibration constants to zero
    for(int i=0; i<16; i++)
    {
        for(int j=0; j<7; j++)
        {
            set_calibration_const(i, j, 0);
        }
    }
    
    // initialize voltages
    for(int i=0; i<16; i++)
    {
        for(int j=0; j<256; j++)
        {
            table[i][j][VOLTAGE] = j*VOLT_INC*1000;
            table[i][j][ENG_VAL] = 0;
        }
    }

    // register callbacks
    bionet_register_callback_datapoint(cb_datapoint);
    hab_register_callback_set_resource(cb_set_resource);

    char full_name[64];
    char hab_name[] = "proxr.cgba5-gse-1.";
    // subscrine to DMM hab calibration constants
    for(int i=0; i<NUM_ADCS; i++)
    {
        // copy hab name in
        strcpy(full_name, hab_name);
        // cat adc calibration
        strcat(full_name, default_settings->adc_calibration[i]);
        for(int j=0; j<NUM_CONSTS; j++)
        {
            char tmp_full_name[64];
            // copy built string from above
            strcpy(tmp_full_name, full_name);
            // cat the -C0 to the resource name
            // resource name is now complete
            strcat(tmp_full_name, default_settings->calibration_const[j]);
            // subscribe to resource
            bionet_subscribe_datapoints_by_name(tmp_full_name);
        }
        // empty full_name
        full_name[0] = '\0';
     }

    // subscribe to DMM hab adc states 
    for(int i=0; i<NUM_STATES; i++)
    {
        // copy hab name into empty full_name
        strcpy(full_name, hab_name);
        // cat the resource name to the hab_name
        strcat(full_name, default_settings->state_names[i]);
        // subscribe
        bionet_subscribe_datapoints_by_name(full_name);
        // make full_name appear empty for next loop
        full_name[0]='\0';
    }
    
    // subscribe to proxr resources
    bionet_subscribe_datapoints_by_name("proxr.cgba5-gse-1.*:*");

    // add node and resources to hab
    create_node(hab, "translator");

    g_message("%s connected to Bionet!", bionet_hab_get_name(hab));
    printf("2 ^ 3 = %f\n", pow(2, 3));
    printf("4.73 ^ 12 = %f\n", pow(4.73,12));
    printf("32.01 ^ 1.54 = %f\n", pow(32.01, 1.54));

    while(1)
    {
        fd_set fds;
        int n, max_fd;
        struct timeval timeout;

        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        
        FD_ZERO(&fds);
        FD_SET(bionet_fd, &fds);
        FD_SET(bionet_hab_fd, &fds);

        max_fd = (bionet_fd > bionet_hab_fd ? bionet_fd : bionet_hab_fd);

        n = select(max_fd+1, &fds, NULL, NULL, &timeout);

        if(n < 0)
            printf("Select failed\n");
        else if(n == 0)
        {
            // printf("TIMEOUT\n");
        }
        else
        {
            if(FD_ISSET(bionet_fd, &fds))
            {
    	        bionet_read_with_timeout(NULL);
            }
    
            if(FD_ISSET(bionet_hab_fd, &fds))
            {
                hab_read_with_timeout(NULL);
            }
        }
    }

    return 0;
}
