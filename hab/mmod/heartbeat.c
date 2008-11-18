
//
// Copyright (C) 2004-2008, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of GNU General Public License version 2, as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//

#include "heartbeat.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "mmod.h"
#include "hardware-abstractor.h"
#include "bionet-hab.h"

#define MAX_MISSED_HEARTBEATS 3

extern bionet_hab_t *this;


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
    bionet_datapoint_t * value;
    if (NULL == this)
    {
	return;
    }

    if (0 > gettimeofday(&tv, NULL))
    {
	g_warning("error with gettimeofday: %s", strerror(errno));
    }

    num_nodes = bionet_hab_get_num_nodes(this);
    
    for (i = 0; i < num_nodes; i++)
    {
	node = bionet_hab_get_node_by_index(this, i);
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
	value = bionet_resource_get_datapoint_by_index(res, num_dps-1);
	hb_time = value->value.uint16_v;
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

	num_dps = bionet_resource_get_num_datapoints(res);
	if (num_dps == 0)
	{
	    continue;
	}
	value = bionet_resource_get_datapoint_by_index(res, num_dps-1);
	(void)timeval_subtract(&diff, &tv, &value->timestamp);
	if (diff.tv_sec > (MAX_MISSED_HEARTBEATS*hb_time))
	{
	    /* this node has been lost! */
	    if (bionet_hab_remove_node_by_id(this, node->id))
	    {
		g_warning("Failed to remove node %s\n", node->id);
	    }
	    if (hab_report_lost_node(node->id))
	    {
		g_warning("Failed to report lost node %s\n", node->id);
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
