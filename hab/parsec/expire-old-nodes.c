
// Copyright (c) 2008-2010, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <sys/time.h>

#include "hardware-abstractor.h"

bionet_hab_t * parsec_hab;

void expire_old_nodes(unsigned int timeout_secs) {
  unsigned int i;

  struct timeval now;

  if (gettimeofday(&now, 0)) {
      g_warning("Failed to get time of day: %m");
      return;
  }

  for (i = 0; i < bionet_hab_get_num_nodes(parsec_hab); i++) {
	bionet_node_t *node;
	bionet_resource_t *resource;
	bionet_datapoint_t *data;
	struct timeval *timeout;
	unsigned long temp_timeout;

	node = bionet_hab_get_node_by_index(parsec_hab, i);
	resource = bionet_node_get_resource_by_id(node, "Temperature");
	data = bionet_resource_get_datapoint_by_index(resource, 0);

	if (data == NULL) {
	  g_warning("Strange, empty datapoint while expiring nodes...");
	  continue;
	}

	timeout = bionet_datapoint_get_timestamp(data);

	if (timeout == NULL) {
	  g_warning("Wierd, timestamp error...");
	  continue;
	}

	// Use a temp variable so we don't overwrite the bionet cache data
	temp_timeout = timeout->tv_sec + timeout_secs;

	// If the timeout is in the future, continue
	if (temp_timeout > now.tv_sec) {
	  continue;
	} else if (timeout->tv_usec == now.tv_usec) {
	  if (timeout->tv_usec > now.tv_usec) {
		continue;
	  }
	}

	// The update is older than the timeout time
	node = bionet_hab_remove_node_by_id(parsec_hab, bionet_node_get_id(node));
	if (hab_report_lost_node(node)) {
	  g_warning("Error reporting a lost node...continuing.");
	}
	bionet_node_free(node);
  }
}
