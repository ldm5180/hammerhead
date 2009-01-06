%module bdm_client
%{
#include "bdm-client.h"
#include "bdm-client-interface.h"
#include "bdm-util.h"
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
%include "bdm-util.h"
%include "bionet-asn.h"
%include "bionet-util.h"
%include "bionet-hab.h"
%include "bionet-node.h"
%include "bionet-resource.h"
%include "bionet-datapoint.h"
%include "bionet-value.h"

typedef unsigned int guint;
typedef void* gpointer;
typedef struct 
{
  gpointer *pdata;
  guint     len;
} GPtrArray;
