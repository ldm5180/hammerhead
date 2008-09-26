
#include <stdio.h>

#include "bdp.h"


void bdp_send_peer_info_ack(cal_peer_t *peer) {
    bdp_peer_user_data_t *user_data = peer->user_data;
    char packet[1024];  // FIXME: max packet size
    int packet_size;

    printf("sending BDP Peer Info Ack\n");

    packet_size = snprintf(
        packet,
        sizeof(packet),
        "BDP Peer Info Ack\n"
    );
    if (packet_size >= sizeof(packet)) {
        printf("error: BDP Peer Info Ack packet too big!  %d bytes\n", packet_size);
        return;
    }

    bdp_unicast_message(packet, packet_size, user_data->addr, user_data->port);
}

