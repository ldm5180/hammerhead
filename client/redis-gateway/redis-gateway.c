#define _XOPEN_SOURCE
#define _BSD_SOURCE

#include <errno.h>
#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <getopt.h>

#include <glib.h>
#include <hiredis/hiredis.h>

#include "bionet.h"
#include "bionet-util.h"

redisContext *redis = NULL;

void signal_handler(int signo) {
  int hi;
  
  for (hi = 0; hi < bionet_cache_get_num_habs(); hi ++) {
    bionet_hab_t *hab = bionet_cache_get_hab_by_index(hi);
    
    // TODO(lenny) Process HABs.
    g_message("    %s", bionet_hab_get_name(hab));
    
    // TODO(lenny) Process Nodes and resources and datapoints.
  }
}

void cb_lost_hab(bionet_hab_t *hab) {
  g_message("lost hab: %s", bionet_hab_get_name(hab));
  if (redis) {
    char buf[1024];
    // TODO(lenny) Make the "bionet" key configurable.
    sprintf(buf, "zrem bionet %s", bionet_hab_get_name(hab));
    redisReply *reply = redisCommand(redis, buf);
    if ( reply->type == REDIS_REPLY_ERROR ) {
      g_message( "Error: %s\n", reply->str );
    }
    freeReplyObject(reply);
  }
}


void cb_new_hab(bionet_hab_t *hab) {
  g_message("new hab: %s", bionet_hab_get_name(hab));
  if (bionet_hab_is_secure(hab)) {
    g_message("    %s: security enabled", bionet_hab_get_name(hab));
  }
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double cur_time = tv.tv_sec * 1000.0 + tv.tv_usec;
  if (redis) {
    char buf[1024];
    // TODO(lenny) Make the "bionet" key configurable.
    sprintf(buf, "zadd bionet %ld %s",
	    (long int)cur_time, bionet_hab_get_name(hab));
    redisReply *reply = redisCommand(redis, buf);
    if ( reply->type == REDIS_REPLY_ERROR ) {
      g_message( "Error: %s\n", reply->str );
    }
    freeReplyObject(reply);
  }
}

int main(int argc, char *argv[]) {
  int bionet_fd;
  
  bionet_log_use_default_handler(NULL);
  
  // TODO(lenny) Parse argument list.
  // TODO(lenny) Initialize security.
  
  bionet_fd = bionet_connect();
  if (bionet_fd < 0) {
    fprintf(stderr, "error connecting to Bionet");
    exit(1);
  }

  // Connect to redis.
  // TODO(lenny) Make the location of redis a command-line option.
  redis = redisConnect("127.0.0.1", 6379);
  if (redis->err) {
    g_message("Error: %s\n", redis->errstr);
  }

  bionet_register_callback_new_hab(cb_new_hab);
  bionet_register_callback_lost_hab(cb_lost_hab);
  
  // TODO(lenny) Subscribe only to specified HABs.
  bionet_subscribe_hab_list_by_name("*.*");
  
  signal(SIGUSR1, signal_handler);
  
  while (1) {
    int r;
    fd_set readers;

    FD_ZERO(&readers);
    FD_SET(bionet_fd, &readers);
    
    r = select(bionet_fd + 1, &readers, NULL, NULL, NULL);
    
    if ((r < 0) && (errno != EINTR)) {
      fprintf(stderr, "error from select: %s", strerror(errno));
      g_usleep(1000*1000);
      break;
    }
    
    bionet_read();  
  } // while(1)

  return 0;
}
