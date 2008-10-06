
#include <pthread.h>

#include "cal.h"


#define BIP_MSG_BUFFER_SIZE (1024)


extern cal_i_t cal_i;


typedef struct {
    cal_peer_t *peer;
    char *topic;
} bip_subscription_request_t;


//
// CAL-I API functions
//

int bip_init_publisher(cal_peer_t *this, void (*callback)(cal_event_t *event));
void bip_publish(char *topic, void *msg, int size);
int bip_publisher_read(void);
int bip_publisher_sendto(cal_peer_t *peer, void *msg, int size);

int bip_init_subscriber(void (*callback)(cal_event_t *event));
void bip_cancel_subscriber(void);
void bip_subscribe(cal_peer_t *peer, char *topic);
int bip_subscriber_read(void);
int bip_subscriber_sendto(cal_peer_t *peer, void *msg, int size);




//
// internal helpers
//

#define Max(a, b) ((a) > (b) ? (a) : (b))


