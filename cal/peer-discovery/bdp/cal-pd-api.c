
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cal.h"
#include "bdp.h"




// the user's callback function
static void (*bdp_user_callback)(cal_event_t *event) = NULL;




// 
//       NAME:  bdp_join()
//
//   FUNCTION:  Joins the BDP network: sends the Join message, and starts
//              the Announcer thread that handles ongoing peer
//              responsibilities (sending heartbeats and responding to Peer
//              Info Requests).
//
//  ARGUMENTS:  The peer that's joining.
//
//    RETURNS:  TRUE (non-zero) on success, FALSE (zero) on failure.
//

static int bdp_join(cal_peer_t *me) {
    char packet[1024];  // FIXME: max packet size
    int packet_size;
    int r;


    //
    // make and send the Join message
    //

    packet_size = snprintf(
        packet,
        sizeof(packet),
        "BDP Join\nPeer-Name: %s%c\n",
        me->name,
        '\0'
    );
    if (packet_size >= sizeof(packet)) {
        printf("bdp_join: error: bdp join packet too big!  %d bytes\n", packet_size);
        return 0;
    }

    printf("bdp_join: sending Join packet\n");
    r = bdp_mcast_message(packet, packet_size);
    if (!r) {
        return 0;
    }


    //
    // start announcer thread
    //

    bdp_announcer_thread = (pthread_t *)malloc(sizeof(pthread_t));
    if (bdp_announcer_thread == NULL) {
        printf("bdp_join: cannot allocate memory for bdp announcer thread: %s\n", strerror(errno));
        return 0;
    }

    r = pthread_create(bdp_announcer_thread, NULL, bdp_announcer_function, me);
    if (r != 0) {
        free(bdp_announcer_thread);
        bdp_announcer_thread = NULL;
        printf("bdp_join: cannot create bdp announcer thread: %s\n", strerror(errno));
        return 0;
    }


    return 1;
}




// 
//       NAME:  bdp_leave()
//
//   FUNCTION:  Leaves the BDP network: kills the Announcer thread and
//              sends the Leave messages.
//
//  ARGUMENTS:  The peer that's leaving.
//
//    RETURNS:  TRUE (non-zero) on success, FALSE (zero) on failure.
//

static int bdp_leave(cal_peer_t *me) {
    char packet[1024];
    int packet_size;
    int r;

    if (bdp_announcer_thread != NULL) {
        printf("bdp_leave: canceling bdp announcer thread\n");
        r = pthread_cancel(*bdp_announcer_thread);
        if (r != 0) {
            printf("bdp_leave: error canceling bdp announcer thread: %s\n", strerror(errno));
        } else {
            bdp_announcer_thread = NULL;
        }
    }

    packet_size = snprintf(
        packet,
        sizeof(packet),
        "BDP Leave\nPeer-Name: %s%c\n",
        me->name,
        '\0'
    );
    if (packet_size >= sizeof(packet)) {
        printf("bdp_leave: error: bdp leave packet too big!  %d bytes\n", packet_size);
        return 0;
    }

    printf("bdp_leave: peer '%s' leaving\n", me->name);
    r = bdp_mcast_message(packet, packet_size);

    close(bdp_mcast_socket);
    close(bdp_unicast_socket);

    return r;
}




// 
//       NAME:  bdp_subscribe_peer_list()
//
//   FUNCTION:  Subscribes to the list of peers on the BDP network: starts
//              the Browser thread that initially probes the network for
//              peers and then continually monitors the network for peers
//              joining and leaving.
//
//  ARGUMENTS:  A callback function to be called (by the read() function
//              below) whenever a Join or Leave event is detected.
//
//    RETURNS:  A file descriptor corresponding to the subscription.  The
//              caller must not read or write the file descriptor, but
//              should monitor it and call bdp_read() when the fd becomes
//              readable.
//

static int bdp_subscribe_peer_list(void (*callback)(cal_event_t *event)) {
    int r;

    // record the user's callback function
    bdp_user_callback = callback;


    // create the pipe for passing events back to the user
    r = pipe(bdp_pipe_fds);
    if (r < 0) {
        printf("bdp_subscribe: error making pipe: %s\n", strerror(errno));
        return -1;
    }

    // start the thread to do the bdp network traffic
    bdp_browser_thread = (pthread_t *)malloc(sizeof(pthread_t));
    if (bdp_browser_thread == NULL) {
        printf("bdp_subscribe: cannot allocate memory for bdp browser thread: %s\n", strerror(errno));
        return -1;
    }
    r = pthread_create(bdp_browser_thread, NULL, bdp_browser_function, NULL);
    if (r != 0) {
        free(bdp_browser_thread);
        bdp_announcer_thread = NULL;
        printf("bdp_subscribe: cannot create bdp browser thread: %s\n", strerror(errno));
        return -1;
    }

    return bdp_pipe_fds[0];
}




// 
//       NAME:  bdp_read()
//
//   FUNCTION:  The user should call this function when the fd returned
//              from bdp_subscribe_peer_list() is readable.  This function
//              reads and parses the network traffic, and calls the user's
//              callback function (registered with
//              bdp_subscribe_peer_list()) as appropriate.
//
//  ARGUMENTS:  None.
//
//    RETURNS:  TRUE (non-zero) on success, FALSE (0) on failure.
//

static int bdp_read(void) {
    int r;
    cal_event_t *event;

    r = read(bdp_pipe_fds[0], &event, sizeof(event));
    if (r < 0) {
        printf("bdp_read: error reading event: %s\n", strerror(errno));
        return 0;
    } else if (r != sizeof(event)) {
        printf("bdp_read: short read while reading event (got %d, expected %lu)\n", r, (unsigned long)sizeof(event));
        return 0;
    } else if (event == NULL) {
        printf("bdp_read: got NULL event!\n");
        return 0;
    }

    printf("bdp_read: got an event %p!\n", event);

    if (bdp_user_callback != NULL) {
        bdp_user_callback(event);
    }

    return 1;
}




// 
//       NAME:  bdp_unsubscribe()
//
//   FUNCTION:  Stops tracking the BDP peer list.  Kills the browser thread
//              started by bdp_subscribe_peer_list().  The fd returned by
//              bdp_subscribe_peer_list() is invalid after this function
//              returns.
//
//  ARGUMENTS:  None.
//
//    RETURNS:  Nothing.
//

static void bdp_unsubscribe_peer_list(void) {
    if (bdp_browser_thread != NULL) {
        int r;

        printf("bdp_unsubscribe: canceling bdp browser thread\n");
        r = pthread_cancel(*bdp_browser_thread);
        if (r != 0) {
            printf("bdp_unsubscribe: error canceling bdp browser thread: %s\n", strerror(errno));
        } else {
            bdp_browser_thread = NULL;
        }
    }

    close(bdp_pipe_fds[0]);
    close(bdp_pipe_fds[1]);

    bdp_user_callback = NULL;

    if (bdp_mcast_socket != -1) {
        close(bdp_mcast_socket);
    }

    if (bdp_unicast_socket != -1) {
        close(bdp_unicast_socket);
    }
}


cal_pd_t cal_pd = {
    .join  = bdp_join,
    .leave = bdp_leave,

    .subscribe_peer_list   = bdp_subscribe_peer_list,
    .read                  = bdp_read,
    .unsubscribe_peer_list = bdp_unsubscribe_peer_list
};

