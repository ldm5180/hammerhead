%module bdm_client
%{
#include "bdm-client.h"
#include "bdm-client-interface.h"
#include "bdm-util.h"
#include "bdm-hab.h"
#include "bdm-node.h"
#include "bdm-resource.h"
#include "bdm-datapoint.h"
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
%include "bdm-asn.h"
%include "bdm-hab.h"
%include "bdm-node.h"
%include "bdm-resource.h"
%include "bdm-datapoint.h"

typedef unsigned int guint;
typedef void* gpointer;
typedef struct 
{
  gpointer *pdata;
  guint     len;
} GPtrArray;



