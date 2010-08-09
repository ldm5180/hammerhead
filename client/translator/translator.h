#include "hardware-abstractor.h"
#include "bionet-util.h"
#include "bionet.h"

#define VOLT_INC .0196078431

#define VOLTAGE 0
#define ENG_VAL 1

//variables
double table[16][256][2];
double calibration_const[16][7];
double adc_increment[16][1];
bionet_resource_t *proxr_resource[16];

// bionet stuff
void cb_datapoint(bionet_datapoint_t *datapoint);
void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value);
void create_node(bionet_hab_t *hab, char *name);
void create_resource(bionet_node_t *node, char *name);

//translation stuff
void set_calibration_const(int adc, int calib, double constant);
void generate_table(int adc);
double find_voltage(int adc, double value);

//misc
void signal_handler(int unused);
void usage(void);
