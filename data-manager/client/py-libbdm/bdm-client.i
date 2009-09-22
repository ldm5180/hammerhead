
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


%module bdm_client
%{
#include "bdm-client.h"
#include "bdm-client-interface.h"
#include "bionet-util.h"
#include "bionet-hab.h"
#include "bionet-node.h"
#include "bionet-resource.h"
#include "bionet-datapoint.h"
#include "bionet-value.h"
%}

typedef unsigned short uint16_t;
 
typedef struct timeval
{
	long int tv_sec;
	long int tv_usec;
};

%include "bdm-client.h"
%include "bdm-client-interface.h"
%include "bionet-asn.h"
%include "bionet-util.h"
%include "bionet-hab.h"
%include "bionet-node.h"
%include "bionet-resource.h"
%include "bionet-datapoint.h"
%include "bionet-value.h"

