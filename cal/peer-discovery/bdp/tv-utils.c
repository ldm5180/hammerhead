
#include <sys/time.h>

#include "bdp.h"


// returns the difference between two timevals, in seconds, as a double
double bdp_tv_diff_double_seconds(const struct timeval *start, const struct timeval *stop) {
   return (stop->tv_sec - start->tv_sec) + ((double)(stop->tv_usec - start->tv_usec) / 1000000.0);
}

