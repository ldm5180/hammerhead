#include "translator.h"

void create_node(bionet_hab_t *hab, char *name)
{
    bionet_node_t *node;
    
    node = bionet_node_new(hab, name);

    // Create 16 adc resources 
    create_adc_resource(node, "adc-00\0");
    create_adc_resource(node, "adc-01\0");
    create_adc_resource(node, "adc-02\0");
    create_adc_resource(node, "adc-03\0");
    create_adc_resource(node, "adc-04\0");
    create_adc_resource(node, "adc-05\0");
    create_adc_resource(node, "adc-06\0");
    create_adc_resource(node, "adc-07\0");
    create_adc_resource(node, "adc-08\0");
    create_adc_resource(node, "adc-09\0");
    create_adc_resource(node, "adc-10\0");
    create_adc_resource(node, "adc-11\0");
    create_adc_resource(node, "adc-12\0");
    create_adc_resource(node, "adc-13\0");
    create_adc_resource(node, "adc-14\0");
    create_adc_resource(node, "adc-15\0");


    // Create min max resources used by pa client to set dials range
    // and store resource for easy access later
    adc_range_resource[0][ZERO_VOLT] = create_range_resource(node, "adc-00-min\0");
    adc_range_resource[0][FIVE_VOLT] = create_range_resource(node, "adc-00-max\0");
    adc_range_resource[1][ZERO_VOLT] = create_range_resource(node, "adc-01-min\0");
    adc_range_resource[1][FIVE_VOLT] = create_range_resource(node, "adc-01-max\0");
    adc_range_resource[2][ZERO_VOLT] = create_range_resource(node, "adc-02-min\0");
    adc_range_resource[2][FIVE_VOLT] = create_range_resource(node, "adc-02-max\0");
    adc_range_resource[3][ZERO_VOLT] = create_range_resource(node, "adc-03-min\0");
    adc_range_resource[3][FIVE_VOLT] = create_range_resource(node, "adc-03-max\0");
    adc_range_resource[4][ZERO_VOLT] = create_range_resource(node, "adc-04-min\0");
    adc_range_resource[4][FIVE_VOLT] = create_range_resource(node, "adc-04-max\0");
    adc_range_resource[5][ZERO_VOLT] = create_range_resource(node, "adc-05-min\0");
    adc_range_resource[5][FIVE_VOLT] = create_range_resource(node, "adc-05-max\0");
    adc_range_resource[6][ZERO_VOLT] = create_range_resource(node, "adc-06-min\0");
    adc_range_resource[6][FIVE_VOLT] = create_range_resource(node, "adc-06-max\0");
    adc_range_resource[7][ZERO_VOLT] = create_range_resource(node, "adc-07-min\0");
    adc_range_resource[7][FIVE_VOLT] = create_range_resource(node, "adc-07-max\0");
    adc_range_resource[8][ZERO_VOLT] = create_range_resource(node, "adc-08-min\0");
    adc_range_resource[8][FIVE_VOLT] = create_range_resource(node, "adc-08-max\0");
    adc_range_resource[9][ZERO_VOLT] = create_range_resource(node, "adc-09-min\0");
    adc_range_resource[9][FIVE_VOLT] = create_range_resource(node, "adc-09-max\0");
    adc_range_resource[10][ZERO_VOLT] = create_range_resource(node, "adc-10-min\0");
    adc_range_resource[10][FIVE_VOLT] = create_range_resource(node, "adc-10-max\0");
    adc_range_resource[11][ZERO_VOLT] = create_range_resource(node, "adc-11-min\0");
    adc_range_resource[11][FIVE_VOLT] = create_range_resource(node, "adc-11-max\0");
    adc_range_resource[12][ZERO_VOLT] = create_range_resource(node, "adc-12-min\0");
    adc_range_resource[12][FIVE_VOLT] = create_range_resource(node, "adc-12-max\0");
    adc_range_resource[13][ZERO_VOLT] = create_range_resource(node, "adc-13-min\0");
    adc_range_resource[13][FIVE_VOLT] = create_range_resource(node, "adc-13-max\0");
    adc_range_resource[14][ZERO_VOLT] = create_range_resource(node, "adc-14-min\0");
    adc_range_resource[14][FIVE_VOLT] = create_range_resource(node, "adc-14-max\0");
    adc_range_resource[15][ZERO_VOLT] = create_range_resource(node, "adc-15-min\0");
    adc_range_resource[15][FIVE_VOLT] = create_range_resource(node, "adc-15-max\0");

    bionet_hab_add_node(hab, node);

    hab_report_new_node(node);
}
