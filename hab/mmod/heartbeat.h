
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef HEARTBEAT_H
#define HEARTBEAT_H


/**
 * Check motes for missing heartbeats
 *
 * When the HAB misses 2 heartbeats from a given mote, it will announce that 
 * mote as a lost node.
 */
void heartbeat_check(void);

#endif /* HEARTBEAT_H */
