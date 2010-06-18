#include "hardware-abstractor.h"

extern bionet_hab_t *hab;
extern int should_exit;

// bionet functions
void add_pot_resource(bionet_node_t *node, int id);
void add_do_resource(bionet_node_t *node, int id);
void add_node(bionet_hab_t *hab, char *name);
void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value);

// misc 
void signal_handler(int unused);
void usage(void);
