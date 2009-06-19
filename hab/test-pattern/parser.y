
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


%{
#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "bionet-util.h"
#include "test-pattern-hab.h"

int yylex();
extern int yyerror();
%}

%start lines

%token ADD
%token REMOVE
%token NOVALUE
%token VALUE
%token FLAVOR
%token TIMEVAL
%token DATATYPE
%token NAME

%union {
    int int_v;
    char *string_v;
    bionet_resource_flavor_t flavor_v;
    bionet_resource_data_type_t datatype_v;
    struct timeval *tv_v;
    GSList *gslist_v;
    struct resource_info_t *resource_info_v;
    struct event_t *event_v;
};

%%

lines: 
    line { events = g_slist_append(events, $<gslist_v>1); } 
  | lines line { events = g_slist_append(events, $<gslist_v>2); }
    ;

line:
    TIMEVAL REMOVE NAME { 
        $<event_v>$ = remove_node_event($<tv_v>1, $<string_v>3); 
    }
  | TIMEVAL ADD NAME { 
        $<event_v>$ = add_node_event($<tv_v>1, $<string_v>3, NULL); 
    }
  | TIMEVAL ADD NAME resources { 
        $<event_v>$ = add_node_event($<tv_v>1, $<string_v>3, $<gslist_v>4); 
    }
  | TIMEVAL NAME NAME VALUE { 
        $<event_v>$ = update_event($<tv_v>1, $<string_v>2, $<string_v>3, $<string_v>4); 
    }
    ;

resources:
    resource { 
        GSList *resources = NULL;
        resources = g_slist_append(resources, $<resource_info_v>1);
        $<gslist_v>$ = resources;
    }
  | resources resource { 
        GSList *resources = $<gslist_v>1;
        resources = g_slist_append(resources, $<resource_info_v>2);
        $<gslist_v>$ = resources;
    }
    ;

resource:
    NAME DATATYPE FLAVOR VALUE { 
        $<resource_info_v>$ = create_resource($<string_v>1, $<datatype_v>2, $<flavor_v>3, $<string_v>4); 
    }
  | NAME DATATYPE FLAVOR NOVALUE { 
        $<resource_info_v>$ = create_empty_resource($<string_v>1, $<datatype_v>2, $<flavor_v>3); 
    }
    ;

