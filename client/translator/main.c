#include <stdlib.h>
#include <stdint.h>
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

    char *hab_type = HAB_TYPE;      // translators hab type
    char *hab_id = NULL;            // translators hab_id
    char *proxr_hab_id = NULL;      // proxr's hab id
    char *dmm_hab_id = NULL;        // dmm's hab id
    char dmm[] = "DMM.";            // dmm's hab type (used for subscribing)
    char proxr[] = "proxr.";        // proxr's hab type (used for subscribing)


    int bionet_fd, bionet_hab_fd, i; 

    while(1)
    {
        int c;
        static struct option long_options[] =
        {
            {"help", 0, 0, '?'},
            {"version", 0, 0, 'v'},
            {"id", 1, 0, 'i'},
            {"proxr", 1, 0, 'p'},
            {"dmm", 1, 0, 'd'},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "?hvp:d:i:", long_options, &i);
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

           case 'p':
                proxr_hab_id = optarg;
                break;

           case 'd':
                dmm_hab_id = optarg;
                break;

            default:
                break;
        }
    }

    if(NULL == proxr_hab_id || NULL == dmm_hab_id)
    {
        usage();
        return 0;
    }

    // read ini file
    int error = translator_read_ini("translator.ini");
    if(0 != error)
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

    // build hash table
    ENTRY e, *ep;
    hcreate(16);
    int r = 0;
    for(; r<16; r++)
    {
        e.key = default_settings->translator_adc[r];
        e.data = &r;
        ep = hsearch(e, ENTER);
        if(ep == NULL)
        {
            fprintf(stderr, "entry to hash table failed\n");
            exit(1);
        }
    }

    // register callbacks
    bionet_register_callback_datapoint(cb_datapoint);
    hab_register_callback_set_resource(cb_set_resource);

    char full_name[64];
    char hab_name[64];
    strcpy(hab_name, dmm);            // DMM.
    strcat(hab_name, dmm_hab_id);     // DMM.hab_id
    strcat(hab_name, ".0:");          // DMM.hab_id.0:

    // subscribe to DMM hab calibration constants
    for(int i=0; i<NUM_DMM_CALIBRATIONS; i++)
    {
        // copy hab name in
        strcpy(full_name, hab_name);
        // cat resource name
        strcat(full_name, default_settings->dmm_calibrations[i]);
        // subscribe to resource
        int ret = bionet_subscribe_datapoints_by_name(full_name);
        if(ret < 0)
            printf("error subscribing to datapoint %s.\n", default_settings->dmm_calibrations[i]);
        // empty full_name for next loop
        full_name[0] = '\0';
     }

     // subscribe to DMM hab adc states 
    for(int i=0; i<NUM_ADCS; i++)
    {
        // copy hab name into empty full_name
        strcpy(full_name, hab_name);
        // cat the resource name to the hab_name
        strcat(full_name, default_settings->state_names[i]);
        // subscribe to resource
        int ret = bionet_subscribe_datapoints_by_name(full_name);
        if(ret < 0)
            printf("error subscribing to datapoint %s.\n", default_settings->state_names[i]);
        // make full_name appear empty for next loop
        full_name[0]='\0';
    }

    hab_name[0] = '\0';
    strcpy(hab_name, proxr);             // proxr.
    strcat(hab_name, proxr_hab_id);      // proxr.hab_id
    strcat(hab_name, ".*:*");            // proxr.hab_id.*:*

    // subscribe to proxr resources
    bionet_subscribe_datapoints_by_name(hab_name);

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
