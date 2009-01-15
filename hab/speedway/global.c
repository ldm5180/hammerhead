
#include "ltkc.h"
#include "hardware-abstractor.h"


GMainLoop *main_loop = NULL;

bionet_hab_t *hab = NULL;
bionet_node_t *reader_node = NULL;

int reader_is_busy = 0;

int show_messages = 0;
int gpi_delay = 0;
int gpi_polarity = 1;

LLRP_tSConnection *pConn;
LLRP_tSTypeRegistry *pTypeRegistry;

