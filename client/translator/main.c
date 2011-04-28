#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>

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
    char dmm[] = "DMM";             // dmm's hab type (used for subscribing)
    char proxr[] = "proxr";         // proxr's hab type (used for subscribing)


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
    // map translator_adc resources to 0-15
    hash_table = g_hash_table_new(g_str_hash, g_str_equal);
    unsigned long int r = 0;
    int jj = 0;
    for(; r<16; r++)
    {
        //unsigned long int x = r;
        // add translator resource names to hash table
        g_hash_table_insert(hash_table, default_settings->translator_adc[r], (gpointer)r);

        // add proxr resource names to hash table
        g_hash_table_insert(hash_table, default_settings->proxr_adc[r], (gpointer)r);

        // map each calibration resource  to 0-15
        // each calibration resource has 7 calibration constants
        int prev_jj = jj;
        for(; jj<prev_jj+7; jj++)
        {
            g_hash_table_insert(hash_table, default_settings->dmm_calibrations[jj], (gpointer)r);
        }
    }

    /*unsigned long int test;
    for(int i=0; i<16; i++)
    {   
        test = (unsigned long int)g_hash_table_lookup(hash_table, default_settings->translator_adc[i]);
        printf("%s mapped to %ld.\n", default_settings->translator_adc[i], test);
	test = (unsigned long int)g_hash_table_lookup(hash_table, default_settings->proxr_adc[i]);
	printf("%s mapped to %ld.\n", default_settings->proxr_adc[i], test);
    }

    for(int i=0; i<112; i++)
    {
	test = (unsigned long int)g_hash_table_lookup(hash_table, default_settings->dmm_calibrations[i]);
	printf("%s mapped to %ld.\n", default_settings->dmm_calibrations[i], test);
    }*/
        

    // register callbacks
    bionet_register_callback_datapoint(cb_datapoint);
    hab_register_callback_set_resource(cb_set_resource);

    int ret;
    char full_name[BIONET_NAME_COMPONENT_MAX_LEN * 4 + 1];

    // subscribe to DMM hab calibration constants
    for(int i=0; i<NUM_DMM_CALIBRATIONS; i++)
    {
        ret = snprintf(full_name, sizeof(full_name), "%s.%s.%d:%s", 
                     dmm, dmm_hab_id, 0, default_settings->dmm_calibrations[i]);
        if (sizeof(full_name) >= ret) {
            fprintf(stderr, "Failed to create full name for resource %s",
                    default_settings->dmm_calibrations[i]);
            continue;
        }

        // subscribe to resource
        ret = bionet_subscribe_datapoints_by_name(full_name);
        if(ret < 0)
            printf("error subscribing to datapoint %s.\n", default_settings->dmm_calibrations[i]);
     }

     // subscribe to DMM hab adc states 
    for(int i=0; i<NUM_ADCS; i++)
    {
        ret = snprintf(full_name, sizeof(full_name), "%s.%s.%d:%s", 
                     dmm, dmm_hab_id, 0, default_settings->state_names[i]);
        if (sizeof(full_name) >= ret) {
            fprintf(stderr, "Failed to create full name for resource %s",
                    default_settings->state_names[i]);
            continue;
        }

        // subscribe to resource
        int ret = bionet_subscribe_datapoints_by_name(full_name);
        if(ret < 0)
            printf("error subscribing to datapoint %s.\n", default_settings->state_names[i]);
    }

    ret = snprintf(full_name, sizeof(full_name), "%s.%s.*.*", 
                 proxr, proxr_hab_id);
    if (sizeof(full_name) >= ret) {
        fprintf(stderr, "Failed to create full name for resource %s",
                default_settings->dmm_calibrations[i]);
    }

    // subscribe to proxr resources
    bionet_subscribe_datapoints_by_name(full_name);

    // add node and resources to hab
    create_node(hab, "translator");

    g_message("%s connected to Bionet!", bionet_hab_get_name(hab));


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
