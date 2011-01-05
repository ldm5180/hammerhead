#include "hardware-abstractor.h"
#include <glib.h>
#include "bionet-util.h"
#include "bionet.h"

#define VOLT_INC .0196078431

#define VOLTAGE 0
#define ENG_VAL 1

#define ZERO_VOLT 0
#define FIVE_VOLT 1

#define NUM_ADCS    16
#define NUM_DMM_CALIBRATIONS 112

// used with ini file
typedef struct
{
    gchar **state_names;
    gchar **dmm_calibrations;
    gchar **translator_adc;
    gchar **proxr_adc;
}translator_settings_t;

//variables
extern translator_settings_t *default_settings;
GHashTable *hash_table;
double table[16][256][2];
double calibration_const[16][7];
double adc_increment[16][1];
bionet_resource_t *adc_range_resource[16][2];
bionet_resource_t *translator_resource[16];
bionet_resource_t *proxr_resource[16];
bionet_resource_t *adc_state_resource[16];

// bionet stuff
void cb_datapoint(bionet_datapoint_t *datapoint);
void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value);
void create_node(bionet_hab_t *hab, char *name);
bionet_resource_t* create_adc_resource(bionet_node_t *node, char *name);
bionet_resource_t* create_range_resource(bionet_node_t *node, char *name);
bionet_resource_t* create_state_resource(bionet_node_t *node, char *name);

//translation stuff
void set_calibration_const(unsigned long int adc, unsigned long int calib, double constant);
void generate_table(unsigned long int adc);
double find_voltage(unsigned long int adc, double value);

//misc
int translator_read_ini(char *config_file);
void signal_handler(int unused);
void usage(void);
