%module bdm_client
%{
#include "bdm-client.h"
#include "bdm-client-interface.h"
#include "bdm-util.h"
#include "bionet-util-2.1.h"
#include "bionet-hab-2.1.h"
#include "bionet-node-2.1.h"
#include "bionet-resource-2.1.h"
#include "bionet-datapoint-2.1.h"
#include "bionet-value-2.1.h"
%}

typedef unsigned short uint16_t;
 
typedef struct timeval
{
	long int tv_sec;
	long int tv_usec;
};

%include "bdm-client.h"
%include "bdm-client-interface.h"
%include "bdm-util.h"
%include "bionet-asn.h"
%include "bionet-util-2.1.h"
%include "bionet-hab-2.1.h"
%include "bionet-node-2.1.h"
%include "bionet-resource-2.1.h"
%include "bionet-datapoint-2.1.h"
%include "bionet-value-2.1.h"

typedef unsigned int guint;
typedef void* gpointer;
typedef struct 
{
  gpointer *pdata;
  guint     len;
} GPtrArray;
