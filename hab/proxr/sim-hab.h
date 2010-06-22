#include "hardware-abstractor.h"
#include "proxrcmds.h"
#include "proxrport.h"
#include "arduinoport.h"

extern bionet_hab_t *hab;
extern GMainLoop *main_loop;

// bionet functions
void add_node(bionet_hab_t *hab, char *name);
void add_pot_resource(bionet_node_t *node, int id);
void add_do_resource(bionet_node_t *node, int id);
void add_ai_resource(bionet_node_t *node, int id);
void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value);
int read_from_bionet(GIOChannel *unused_channel, GIOCondition unused_condition, gpointer unused_data);

// hardware stuff
int poll_arduino();

// misc 
void signal_handler(int unused);
void usage(void);
