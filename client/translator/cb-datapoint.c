#include "translator.h"
#include <stdlib.h>

void cb_datapoint(bionet_datapoint_t *datapoint)
{
    bionet_resource_t *resource = NULL;
    char *res_name = NULL;
    char id[3];
    long int adc_id, calib_id, pot_id;
    double constant;
    
    resource = bionet_datapoint_get_resource(datapoint);
    bionet_split_resource_name(bionet_resource_get_name(resource), NULL, NULL, NULL, &res_name);

    // if the resource name begins with 'a' it is from DMM hab if 'p' it is from proxr hab.
    if(res_name[0] == 'a')
    {
        // use resource name to find adc number (0-15)
        id[0] = res_name[3];
        id[1] = res_name[4];
        id[2] = '\0';   
        adc_id = strtol(id, NULL, 10);
    
        // use resource name to find calibration constant number (0-6)
        id[0] = res_name[19];
        id[1] = '\0';
        calib_id = strtol(id, NULL, 10);

        // get content from value
        bionet_resource_get_double(resource, &constant, NULL);

        // set calibration constant and update engineering value table
        set_calibration_const(adc_id, calib_id, constant);

        // update min and max resource value
        //convert double value to char*
        char *buff = NULL;
        sprintf(buff, "%f", table[adc_id][0][ENG_VAL]);
        bionet_set_resource(adc_range_resource[adc_id][ZERO_VOLT], buff);
        sprintf(buff, "%f", table[adc_id][255][ENG_VAL]);
        bionet_set_resource(adc_range_resource[adc_id][FIVE_VOLT], buff);
    }
    else if(res_name[0] == 'p')
    {
        // extract pot number from resource name (0-15)
        id[0] = res_name[4];
        id[1] = res_name[5];
        id[2] = '\0';
        pot_id = strtol(id, NULL, 10); 

        proxr_resource[pot_id] = resource;
    }
}
