
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cal-dnssd.h"




// 
//       NAME:  cal_pd_dnssd_subscribe_peer_list()
//
//   FUNCTION:  Subscribes to the list of peers on the network: starts
//              the Browser thread that initially probes the network for
//              peers and then continually monitors the network for peers
//              joining and leaving.
//
//  ARGUMENTS:  A callback function to be called (by the read() function
//              below) whenever a Join or Leave event is detected.
//
//    RETURNS:  A file descriptor corresponding to the subscription.  The
//              caller must not read or write the file descriptor, but
//              should monitor it and call cal_pd_dnssd_read() when the fd
//              becomes readable.
//

static int cal_pd_dnssd_subscribe_peer_list(void (*callback)(cal_event_t *event)) {
    int r;

    // create the pipe for passing events back to the user
    r = pipe(cal_pd_dnssd_browser_fds);
    if (r < 0) {
        printf("dnssd subscribe_peer_list: error making pipe: %s\n", strerror(errno));
        return -1;
    }

    cal_pd_dnssd_browser_thread = (pthread_t *)malloc(sizeof(pthread_t));
    if (cal_pd_dnssd_browser_thread == NULL) {
        printf("dnssd subscribe_peer_list: cannot allocate memory for browser thread: %s\n", strerror(errno));
        close(cal_pd_dnssd_browser_fds[0]);
        close(cal_pd_dnssd_browser_fds[1]);
        return -1;
    }

    // record the user's callback function
    cal_pd_dnssd_callback = callback;

    // start the browser thread to talk to the mDNS server
    r = pthread_create(cal_pd_dnssd_browser_thread, NULL, cal_pd_dnssd_browser_function, NULL);
    if (r != 0) {
        free(cal_pd_dnssd_browser_thread);
        cal_pd_dnssd_browser_thread = NULL;
        cal_pd_dnssd_callback = NULL;
        printf("dnssd subscribe_peer_list: cannot create browser thread: %s\n", strerror(errno));
        return -1;
    }

    return cal_pd_dnssd_browser_fds[0];
}




// 
//       NAME:  cal_pd_dnssd_read()
//
//   FUNCTION:  The user should call this function when the fd returned
//              from cal_pd_dnssd_subscribe_peer_list() is readable.  This
//              function reads and parses the network traffic, and calls
//              the user's callback function (registered with
//              cal_pd_dnssd_subscribe_peer_list()) as appropriate.
//
//  ARGUMENTS:  None.
//
//    RETURNS:  TRUE (non-zero) on success, FALSE (0) on failure.
//

static int cal_pd_dnssd_read(void) {
    int r;
    cal_event_t *event;

    r = read(cal_pd_dnssd_browser_fds[0], &event, sizeof(event));
    if (r < 0) {
        printf("cal_pd_dnssd_read: error reading event: %s\n", strerror(errno));
        return 0;
    } else if (r != sizeof(event)) {
        printf("cal_pd_dnssd_read: short read while reading event (got %d, expected %lu)\n", r, (unsigned long)sizeof(event));
        return 0;
    } else if (event == NULL) {
        printf("cal_pd_dnssd_read: got NULL event!\n");
        return 0;
    }

    if (cal_pd_dnssd_callback != NULL) {
        cal_pd_dnssd_callback(event);
    }


    // manage memory
    switch (event->type) {
        case CAL_EVENT_JOIN: {
            event->peer = NULL;  // the CAL-PD subscriber thread has a copy, we'll free it when the peer leaves later
            break;
        }

        case CAL_EVENT_LEAVE: {
            break;
        }

        default: {
            fprintf(stderr, "cal_pd_dnssd_read(): got unhandled event type %d\n", event->type);
            break;
        }
    }

    cal_event_free(event);

    return 1;
}




// 
//       NAME:  cal_pd_dnssd_unsubscribe()
//
//   FUNCTION:  Stops tracking the peer list.  Kills the browser thread
//              started by cal_pd_dnssd_subscribe_peer_list().  The fd
//              returned by cal_pd_dnssd_subscribe_peer_list() is invalid
//              after this function returns.
//
//  ARGUMENTS:  None.
//
//    RETURNS:  Nothing.
//

static void cal_pd_dnssd_unsubscribe_peer_list(void) {
    if (cal_pd_dnssd_browser_thread != NULL) {
        int r;

        // printf("cal_pd_dnssd_unsubscribe: canceling browser thread\n");
        r = pthread_cancel(*cal_pd_dnssd_browser_thread);
        if (r != 0) {
            printf("cal_pd_dnssd_unsubscribe: error canceling browser thread: %s\n", strerror(errno));
            return;
        } else {
            // printf("cal_pd_dnssd_unsubscribe: browser thread canceled\n");
            pthread_join(*cal_pd_dnssd_browser_thread, NULL);
            free(cal_pd_dnssd_browser_thread);
            cal_pd_dnssd_browser_thread = NULL;
        }
    }

    cal_pd_dnssd_end_browse();

    close(cal_pd_dnssd_browser_fds[0]);
    close(cal_pd_dnssd_browser_fds[1]);

    cal_pd_dnssd_callback = NULL;
}




// 
// here's the cal-pd struct that exports this module to the caller
//

cal_pd_t cal_pd = {
    .join = dnssd_register,
    .leave = dnssd_remove,

    .subscribe_peer_list = cal_pd_dnssd_subscribe_peer_list,
    .read = cal_pd_dnssd_read,
    .unsubscribe_peer_list = cal_pd_dnssd_unsubscribe_peer_list
};

