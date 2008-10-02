
#include "cal.h"
#include "bip.h"


// FIXME: this one's used by the publisher too
void (*cal_i_bip_subscriber_callback)(cal_event_t *event) = NULL;


// FIXME: this one's used by the publisher
int bip_socket = -1;

