#include "translator.h"

void create_node(bionet_hab_t *hab, char *name)
{
    bionet_node_t *node;
    
    node = bionet_node_new(hab, name);

    // Create 16 adc resources and store for easy access later 
    translator_resource[0] = create_adc_resource(node, "adc-00");
    translator_resource[1] = create_adc_resource(node, "adc-01");
    translator_resource[2] = create_adc_resource(node, "adc-02");
    translator_resource[3] = create_adc_resource(node, "adc-03");
    translator_resource[4] = create_adc_resource(node, "adc-04");
    translator_resource[5] = create_adc_resource(node, "adc-05");
    translator_resource[6] = create_adc_resource(node, "adc-06");
    translator_resource[7] = create_adc_resource(node, "adc-07");
    translator_resource[8] = create_adc_resource(node, "adc-08");
    translator_resource[9] = create_adc_resource(node, "adc-09");
    translator_resource[10] = create_adc_resource(node, "adc-10");
    translator_resource[11] = create_adc_resource(node, "adc-11");
    translator_resource[12] = create_adc_resource(node, "adc-12");
    translator_resource[13] = create_adc_resource(node, "adc-13");
    translator_resource[14] = create_adc_resource(node, "adc-14");
    translator_resource[15] = create_adc_resource(node, "adc-15");


    // Create min max resources used by pa client to set dials range
    // and store resource for easy access later
    adc_range_resource[0][ZERO_VOLT] = create_range_resource(node, "adc-00-min");
    adc_range_resource[0][FIVE_VOLT] = create_range_resource(node, "adc-00-max");
    adc_range_resource[1][ZERO_VOLT] = create_range_resource(node, "adc-01-min");
    adc_range_resource[1][FIVE_VOLT] = create_range_resource(node, "adc-01-max");
    adc_range_resource[2][ZERO_VOLT] = create_range_resource(node, "adc-02-min");
    adc_range_resource[2][FIVE_VOLT] = create_range_resource(node, "adc-02-max");
    adc_range_resource[3][ZERO_VOLT] = create_range_resource(node, "adc-03-min");
    adc_range_resource[3][FIVE_VOLT] = create_range_resource(node, "adc-03-max");
    adc_range_resource[4][ZERO_VOLT] = create_range_resource(node, "adc-04-min");
    adc_range_resource[4][FIVE_VOLT] = create_range_resource(node, "adc-04-max");
    adc_range_resource[5][ZERO_VOLT] = create_range_resource(node, "adc-05-min");
    adc_range_resource[5][FIVE_VOLT] = create_range_resource(node, "adc-05-max");
    adc_range_resource[6][ZERO_VOLT] = create_range_resource(node, "adc-06-min");
    adc_range_resource[6][FIVE_VOLT] = create_range_resource(node, "adc-06-max");
    adc_range_resource[7][ZERO_VOLT] = create_range_resource(node, "adc-07-min");
    adc_range_resource[7][FIVE_VOLT] = create_range_resource(node, "adc-07-max");
    adc_range_resource[8][ZERO_VOLT] = create_range_resource(node, "adc-08-min");
    adc_range_resource[8][FIVE_VOLT] = create_range_resource(node, "adc-08-max");
    adc_range_resource[9][ZERO_VOLT] = create_range_resource(node, "adc-09-min");
    adc_range_resource[9][FIVE_VOLT] = create_range_resource(node, "adc-09-max");
    adc_range_resource[10][ZERO_VOLT] = create_range_resource(node, "adc-10-min");
    adc_range_resource[10][FIVE_VOLT] = create_range_resource(node, "adc-10-max");
    adc_range_resource[11][ZERO_VOLT] = create_range_resource(node, "adc-11-min");
    adc_range_resource[11][FIVE_VOLT] = create_range_resource(node, "adc-11-max");
    adc_range_resource[12][ZERO_VOLT] = create_range_resource(node, "adc-12-min");
    adc_range_resource[12][FIVE_VOLT] = create_range_resource(node, "adc-12-max");
    adc_range_resource[13][ZERO_VOLT] = create_range_resource(node, "adc-13-min");
    adc_range_resource[13][FIVE_VOLT] = create_range_resource(node, "adc-13-max");
    adc_range_resource[14][ZERO_VOLT] = create_range_resource(node, "adc-14-min");
    adc_range_resource[14][FIVE_VOLT] = create_range_resource(node, "adc-14-max");
    adc_range_resource[15][ZERO_VOLT] = create_range_resource(node, "adc-15-min");
    adc_range_resource[15][FIVE_VOLT] = create_range_resource(node, "adc-15-max");

    // Create state resources used by pa client
    adc_state_resource[0] = create_state_resource(node, "adc-00-state");
    adc_state_resource[1] = create_state_resource(node, "adc-01-state");
    adc_state_resource[2] = create_state_resource(node, "adc-02-state");
    adc_state_resource[3] = create_state_resource(node, "adc-03-state");
    adc_state_resource[4] = create_state_resource(node, "adc-04-state");
    adc_state_resource[5] = create_state_resource(node, "adc-05-state");
    adc_state_resource[6] = create_state_resource(node, "adc-06-state");
    adc_state_resource[7] = create_state_resource(node, "adc-07-state");
    adc_state_resource[8] = create_state_resource(node, "adc-08-state");
    adc_state_resource[9] = create_state_resource(node, "adc-09-state");
    adc_state_resource[10] = create_state_resource(node, "adc-10-state");
    adc_state_resource[11] = create_state_resource(node, "adc-11-state");
    adc_state_resource[12] = create_state_resource(node, "adc-12-state");
    adc_state_resource[13] = create_state_resource(node, "adc-13-state");
    adc_state_resource[14] = create_state_resource(node, "adc-14-state");
    adc_state_resource[15] = create_state_resource(node, "adc-15-state");


    bionet_hab_add_node(hab, node);

    hab_report_new_node(node);
}
