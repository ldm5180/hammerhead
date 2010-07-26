#include "hardware-abstractor.h"
#include "arduinoport.h"

#define ANALOG_INPUT_CONVERSION 0.004887586

#define DIGITAL_INPUT_0 0
#define DIGITAL_INPUT_1 1
#define DIGITAL_INPUT_2 2
#define DIGITAL_INPUT_3 3
#define DIGITAL_INPUT_4 4
#define DIGITAL_INPUT_5 5
#define DIGITAL_INPUT_6 6
#define DIGITAL_INPUT_7 7

#define ANALOG_INPUT_0 8
#define ANALOG_INPUT_1 9

extern bionet_hab_t *hab;

// bionet functions
void add_node(bionet_hab_t *hab, char *name);
void add_di_resource(bionet_node_t *node, int id);
void add_ai_resource(bionet_node_t *node, int id);

// hardware stuff
int poll_arduino(int bionet_fd);

// misc 
void signal_handler(int unused);
void usage(void);
