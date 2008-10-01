
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "cal.h"
#include "glib-on-cal.h"


// link in a provider of cal_pd 
extern cal_pd_t cal_pd;


cal_peer_t this_peer;


void (*my_cb_new_hab)(bionet_hab_t *new_hab) = NULL;
void (*my_cb_lost_hab)(bionet_hab_t *lost_hab) = NULL;


int join(bionet_hab_t *this_hab) {
    this_peer.name = strdup(this_hab->name);
    return cal_pd.join(&this_peer);
}


int leave(bionet_hab_t *me) {
    return cal_pd.leave(&this_peer);
}


static void callback(cal_event_t *event) {
    switch (event->event_type) {
        case CAL_EVENT_JOIN: {
            printf("got JOIN event\n");
            if (my_cb_new_hab != NULL) {
                bionet_hab_t hab;
                hab.name = event->peer->name;
                my_cb_new_hab(&hab);
            }
            break;
        }

        case CAL_EVENT_LEAVE: {
            printf("got LEAVE event\n");
            if (my_cb_lost_hab != NULL) {
                bionet_hab_t hab;
                hab.name = event->peer->name;
                my_cb_lost_hab(&hab);
            }
            break;
        }

        default: {
            printf("unknown event type %d\n", event->event_type);
            break;
        }
    }

    cal_event_free(event);  // don't forget!
}

static gboolean readable_handler(GIOChannel *ch, GIOCondition cond, gpointer unused) {
    printf("glib-on-cal read handler running\n");

    if (cond == G_IO_IN) {
        if (!cal_pd.read()) {
            printf("error reading event: %s\n", strerror(errno));
            cal_pd.unsubscribe_peer_list();
            return FALSE;
        }
        return TRUE;
    } else {
        printf("glib-on-cal read handler called for unknown condition\n");
        return FALSE;
    }
}


int subscribe_hab_list(
    void (*cb_new_hab)(bionet_hab_t *new_hab),
    void (*cb_lost_hab)(bionet_hab_t *lost_hab)
) {
    int fd;
    GIOChannel *ch;

    fd = cal_pd.subscribe_peer_list(callback);
    if (fd < 0) {
        printf("error making event-fd for cal-pd subscription\n");
        return 0;
    }

    // record the callbacks
    my_cb_new_hab = cb_new_hab;
    my_cb_lost_hab = cb_lost_hab;

    // if the socket is readable, call the cal->read function
    ch = g_io_channel_unix_new(fd);
    g_io_add_watch(ch, (G_IO_IN | G_IO_ERR | G_IO_HUP | G_IO_NVAL), (GIOFunc)readable_handler, NULL);

    return 1;
}

