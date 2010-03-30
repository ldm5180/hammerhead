
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "config.h"

#include "../bip-itc.h"

#include <errno.h>
#include <glib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int bip_msg_queue_init(bip_msg_queue_t *q) {
    int r;
    r = pipe(q->to_user);
    if(r < 0 ) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "init: error making to-user pipe: %s", strerror(errno));
        return -1;
    }

    r = pipe(q->from_user);
    if(r < 0 ) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "init: error making from-user pipe: %s", strerror(errno));
        return -1;
    }

    return 0;
}

void bip_msg_queue_ref(bip_msg_queue_t *q) {

}


void bip_msg_queue_unref(bip_msg_queue_t *q) {
    int i;
    for(i=0;i<2;i++) {
        if(q->to_user[i] >= 0) {
            int r = close(q->to_user[i]);
            if ( r < 0 ) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "%s: error closing pipe %s", __FUNCTION__, strerror(errno));
            }
        }

        if(q->from_user[i] >= 0) {
            int r = close(q->from_user[i]);
            if ( r < 0 ) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "%s: error closing pipe %s", __FUNCTION__, strerror(errno));
            }
        }
    }
}

int bip_msg_queue_close(bip_msg_queue_t *q, bip_msg_queue_direction_t dir) {
    int *fd;
    if(dir == BIP_MSG_QUEUE_TO_USER){
        fd = &q->to_user[1];
    } else {
        fd = &q->from_user[1];
    }

    if(*fd >= 0) {
        int r = close(*fd);
        if ( r < 0 ) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s: error closing pipe %s", __FUNCTION__, strerror(errno));
            return r;
        }
        *fd = -1;
    }
    return 0;
}

int bip_msg_queue_push(bip_msg_queue_t *q, bip_msg_queue_direction_t dir, cal_event_t * event) {
    int r;

    int fd;
    if(dir == BIP_MSG_QUEUE_TO_USER){
        fd = q->to_user[1];
    } else {
        fd = q->from_user[1];
    }

    r = write(fd, &event, sizeof(cal_event_t*));
    if ( r < 0 ) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "%s: error writing to pipe: %s", __FUNCTION__, strerror(errno));
        return -1;
    }
    if ( r != sizeof(cal_event_t*) ) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "%s: short write to pipe", __FUNCTION__);
        return -1;
    }

    return 0;
}


int bip_msg_queue_pop(bip_msg_queue_t *q, bip_msg_queue_direction_t dir, cal_event_t ** event) {
    int r;

    int fd;
    if(dir == BIP_MSG_QUEUE_TO_USER){
        fd = q->to_user[0];
    } else {
        fd = q->from_user[0];
    }

    r = read(fd, event, sizeof(cal_event_t*));

    if (r == sizeof(cal_event_t*) ) {
        return 0;
    } 
    if (r == 0 ) {
        *event = NULL;
        return 1;
    }
    
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s: error reading from pipe: %s", 
                __FUNCTION__, strerror(errno));
    }

    *event = NULL;
    return -1;

}

#if 0
int bip_msg_queue_set_blocking(bip_msg_queue_t q, int blocking) {
    long flags = 0;
    flags = fcntl(cal_server_mdnssd_bip_fds_to_user[0], F_GETFL, 0);
    if (flags < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "init: cannot get flags for cal_fd: %s", strerror(errno));
        return -1;
    }

    if(blocking) {
        flags &= ~O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }

    r = fcntl(cal_server_mdnssd_bip_fds_to_user[0], F_SETFL, O_NONBLOCK);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "init: cannot set cal_fd flags: %s", strerror(errno));
        return -1;
    }


    return 0;
}
#endif

