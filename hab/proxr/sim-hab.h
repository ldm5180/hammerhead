#include "hardware-abstractor.h"
#include "proxrcmds.h"
#include "proxrport.h"

#define POT_CONVERSION 0.019607843

extern bionet_hab_t *hab;

// bionet functions
void add_node(bionet_hab_t *hab, char *name);
void add_pot_resource(bionet_node_t *node, char* name);
void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value);

// misc 
void signal_handler(int unused);
void usage(void);
