
#include "cal-client-mdnssd-bip.h"


void (*cal_client_mdnssd_bip_callback)(cal_event_t *event) = NULL;

int cal_client_mdnssd_bip_fds_to_user[2];
int cal_client_mdnssd_bip_fds_from_user[2];

GSList *cal_client_mdnssd_bip_service_list = NULL;

pthread_t *cal_client_mdnssd_bip_thread = NULL;

