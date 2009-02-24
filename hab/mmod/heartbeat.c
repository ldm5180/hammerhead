
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "heartbeat.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "mmod.h"
#include "hardware-abstractor.h"
#include "bionet-hab.h"

#define MAX_MISSED_HEARTBEATS 3

extern bionet_hab_t *mmod_hab;


int timeval_subtract (struct timeval *result, 
		      struct timeval *x, 
		      struct timeval *y);


void heartbeat_check(void)
{
    int num_nodes, i;
    bionet_node_t *node;
    bionet_resource_t *res;
    struct timeval tv;
    struct timeval diff;
    uint16_t hb_time;
    int num_dps;
    bionet_value_t * value;
    bionet_datapoint_t * datapoint;

    if (NULL == mmod_hab)
    {
	return;
    }

    if (0 > gettimeofday(&tv, NULL))
    {
	g_warning("error with gettimeofday: %s", strerror(errno));
    }

    num_nodes = bionet_hab_get_num_nodes(mmod_hab);
    
    for (i = 0; i < num_nodes; i++)
    {
	node = bionet_hab_get_node_by_index(mmod_hab, i);
	if(NULL == node)
	{
	    continue;
	}

	/* get the heartbeat time for this node */
	res = bionet_node_get_resource_by_id(node, "HeartbeatTime");
	if (NULL == res)
	{
	    continue;
	}

	num_dps = bionet_resource_get_num_datapoints(res);
	if (num_dps == 0)
	{
	    continue;
	}
	datapoint = bionet_resource_get_datapoint_by_index(res, 0);
	value = bionet_datapoint_get_value(datapoint);
	if (bionet_value_get_uint16(value, &hb_time))
	{
	    g_warning("Failed to get heartbeat time from datapoint");
	}
	if (0 == hb_time)
	{
	    /* if this node hasn't yet published it's heartbeat, use the max */
	    hb_time = 0xFFFF/MAX_MISSED_HEARTBEATS;
	}

	/* get a resource which gets published frequently and verify its
	 * update time isn't too old */
	res = bionet_node_get_resource_by_id(node, "Accel-X");
	if (NULL == res)
	{
	    continue;
	}

	(void)timeval_subtract(&diff, 
			       &tv, bionet_datapoint_get_timestamp(datapoint));
	if (diff.tv_sec > (MAX_MISSED_HEARTBEATS*hb_time))
	{
	    /* this node has been lost! */
	    const char * node_id = bionet_node_get_id(node);
	    bionet_node_t * node;
	    node = bionet_hab_remove_node_by_id(mmod_hab, node_id);
	    if (hab_report_lost_node(node_id))
	    {
		g_warning("Failed to report lost node %s\n", node_id);
	    }
	    if (NULL == node)
	    {
		g_warning("Failed to remove node %s\n", node_id);
	    }
	    else
	    {
		bionet_node_free(node);
	    }
	}
    }
} /* heartbeat_check() */


int timeval_subtract (struct timeval *result, 
		      struct timeval *x, 
		      struct timeval *y)
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) 
    {
	int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
	y->tv_usec -= 1000000 * nsec;
	y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) 
    {
	int nsec = (x->tv_usec - y->tv_usec) / 1000000;
	y->tv_usec += 1000000 * nsec;
	y->tv_sec -= nsec;
    }
    
    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;
    
    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
} /* timeval_subtract() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
