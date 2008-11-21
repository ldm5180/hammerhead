
#include "cal-server-mdnssd-bip.h"


char *cal_server_mdnssd_bip_network_type = NULL;

void (*cal_server_mdnssd_bip_callback)(cal_event_t *event) = NULL;

// pipes between CAL Server thread and user thread
int cal_server_mdnssd_bip_fds_to_user[2];
int cal_server_mdnssd_bip_fds_from_user[2];

pthread_t *cal_server_mdnssd_bip_thread = NULL;

