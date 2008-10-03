
#include "cal.h"
#include "bip.h"


cal_i_t cal_i = {
    .publisher_callback = NULL,
    .init_publisher = bip_init_publisher,
    .publish = bip_publish,
    .publisher_read = bip_publisher_read,

    .subscriber_callback = NULL,
    .init_subscriber = bip_init_subscriber,
    .cancel_subscriber = bip_cancel_subscriber,
    .subscribe = bip_subscribe,
    .subscriber_read = bip_subscriber_read,

    .sendto = bip_sendto
};

