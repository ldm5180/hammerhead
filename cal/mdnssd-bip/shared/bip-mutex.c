
// Copyright (c) 2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <pthread.h>
#include "cal-mdnssd-bip.h"

void cal_pthread_mutex_lock(pthread_mutex_t * mutex) {
    while (1)
    {
	int                status; 
	struct timespec    delay; 
	struct timespec    remtime;
	
	status = pthread_mutex_lock(mutex); 
        if (0 != status) 
	{ 
	    if ((status == EINVAL) || (status == EDEADLK)) 
	    { 
		g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "LOCKING: Invalid or deadlocked mutex!\n"); 
		break; 
	    } 
	    
	    delay.tv_sec = 0; 
	    delay.tv_nsec = 100000; 
	    remtime.tv_sec = 0; 
	    remtime.tv_nsec = 0; 
	    (void) nanosleep (&delay, &remtime); 
	    continue; 
	} 
	break; 
    }
}

void cal_pthread_mutex_unlock(pthread_mutex_t * mutex) {
    while (1) 
    { 
	int                status; 
	struct timespec    delay; 
	struct timespec    remtime; 
	
	status = pthread_mutex_unlock(mutex); 
        if (0 != status) 
	{ 
	    if ((status == EINVAL) || (status == EDEADLK)) 
	    { 
		g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "UNLOCKING: Invalid or deadlocked mutex!\n"); 
		break; 
	    } 
	    
	    delay.tv_sec = 0; 
	    delay.tv_nsec = 100000; 
	    remtime.tv_sec = 0; 
	    remtime.tv_nsec = 0; 
	    (void) nanosleep (&delay, &remtime); 
	    
	    continue; 
	} 
	break; 
    }
}

