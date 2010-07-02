#include "hardware-abstractor.h"
#include "proxrcmds.h"
#include "proxrport.h"
#include "arduinoport.h"

#define ANALOG_INPUT_CONVERSION 0.004887586
#define POT_CONVERSION 0.019607843

#define ANALOG_INPUT_0 24
#define ANALOG_INPUT_1 25

#define DIGITAL_INPUT_0 16
#define DIGITAL_INPUT_1 17
#define DIGITAL_INPUT_2 18
#define DIGITAL_INPUT_3 19
#define DIGITAL_INPUT_4 20
#define DIGITAL_INPUT_5 21
#define DIGITAL_INPUT_6 22
#define DIGITAL_INPUT_7 23

extern bionet_hab_t *hab;
extern GMainLoop *main_loop;

// bionet functions
void add_node(bionet_hab_t *hab, char *name);
void add_pot_resource(bionet_node_t *node, int id);
void add_di_resource(bionet_node_t *node, int id);
void add_ai_resource(bionet_node_t *node, int id);
void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value);
int read_from_bionet(GIOChannel *unused_channel, GIOCondition unused_condition, gpointer unused_data);

// hardware stuff
int poll_arduino(int bionet_fd);

// misc 
void signal_handler(int unused);
void usage(void);
