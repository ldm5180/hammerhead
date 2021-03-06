
// Copyright (c) 2008-2010, Gary Grobe and the Regents of the University of
// Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>

#include "ltkc.h"
#include "speedway.h"

/*
 * Try to reset the configuration to factory defaults. This feature is 
 * optional and the reader may not support it. Then delete all ROSpecs.
 */

int scrubConfiguration() {

	if (resetConfigurationToFactoryDefaults() != 0) {
		return -1;
	}

	if (deleteAllROSpecs() != 0) {
		return -2;
	}

	return 0;
}


