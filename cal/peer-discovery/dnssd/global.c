
#include "cal-dnssd.h"


const char *cal_pd_dnssd_service_name = "_bionet._tcp";

pthread_t *cal_pd_dnssd_browser_thread = NULL;
int cal_pd_dnssd_browser_fds[2];

void (*cal_pd_dnssd_callback)(cal_event_t *event) = NULL;

