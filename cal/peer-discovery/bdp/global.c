
#include "bdp.h"


int bdp_mcast_socket = -1;
int bdp_unicast_socket = -1;


// NULL means: thread not running
pthread_t *bdp_announcer_thread = NULL;
pthread_t *bdp_browser_thread = NULL;


int bdp_pipe_fds[2] = { -1, -1 };

