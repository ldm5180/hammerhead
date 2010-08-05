
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


%{
#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#include "bionet-data-manager.h"
#include "bdm-db.h"

int yylex();
extern int yyerror();
extern FILE * yyin;

static sync_sender_config_t * bdmcfg;

%}

%start opts

%token SET_METHOD
%token SET_START
%token SET_STOP
%token SET_RES
%token SET_FREQ
%token SET_RECPT
%token SET_PORT
%token SET_BUNDLE_LIFETIME
%token ASSIGN
%token OPTWHITE
%token STRINGVAL
%token TIMEVAL
%token INTVAL
%token METH_TCP
%token METH_ION

%token SET_UNKNOWN

%union {
    int intval;
    char *stringval;
    struct timeval timeval;
}

%%

opts    :   opt
        |   opts opt
        ;


opt     :       set_method
        |       set_start
        |       set_stop
        |       set_res
        |       set_recpt_port
        |       set_recpt
        |       set_freq
        |       set_bundle_lifetime
        |       set_unknown
        ;

set_method  :   SET_METHOD ASSIGN METH_TCP { 
                    bdmcfg->method = BDM_SYNC_METHOD_TCP;
                }
            |   SET_METHOD ASSIGN METH_ION {
                    bdmcfg->method = BDM_SYNC_METHOD_ION;
                }
            ;

set_start       :       SET_START ASSIGN TIMEVAL {
                            bdmcfg->start_time.tv_sec = $<timeval>3.tv_sec;
                            bdmcfg->start_time.tv_usec = $<timeval>3.tv_usec;
                        }

set_stop        :       SET_STOP ASSIGN TIMEVAL {
                            bdmcfg->end_time.tv_sec = $<timeval>3.tv_sec;
                            bdmcfg->end_time.tv_usec = $<timeval>3.tv_usec;
                        }

set_res         :       SET_RES ASSIGN STRINGVAL {
                            strncpy(
                                bdmcfg->resource_name_pattern,
                                $<stringval>3,
                                sizeof(bdmcfg->resource_name_pattern) - 1);
			    free($<stringval>3);
                        }

set_freq        :       SET_FREQ ASSIGN INTVAL {
                            bdmcfg->frequency = $<intval>3;
                        }

set_recpt_port :        SET_PORT ASSIGN INTVAL {
                            bdmcfg->remote_port = $<intval>3;
                        }

set_recpt        :      SET_RECPT ASSIGN STRINGVAL {
                            if(bdmcfg->sync_recipient){
                                free(bdmcfg->sync_recipient);
                            }
                            bdmcfg->sync_recipient = $<stringval>3;
                        }

set_bundle_lifetime :   SET_BUNDLE_LIFETIME ASSIGN INTVAL {
                            bdmcfg->bundle_lifetime = $<intval>3;
                        }

set_unknown     :       SET_UNKNOWN ASSIGN STRINGVAL {
                            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                                "Unknown setting '%s'. Ignoring\n",
                                $<stringval>1);
			    free($<stringval>3);
                        }


%%

//
// Parse the config file, and populate the data structure.
// NOTE: This method is not thread safe, as it uses static memory
// (Blame yacc/lex)
//
sync_sender_config_t * read_config_file(const char * fname) {

    yyin = fopen(fname, "r");
    if(NULL == yyin) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "error opening sync-send config file '%s': %s\n",
            fname, g_strerror(errno));
        return NULL;
    }

    sync_sender_config_t * cfg = calloc(1, sizeof(sync_sender_config_t));

    if(cfg){
        // Set default config values first
        cfg->remote_port = BDM_SYNC_PORT;
        cfg->bundle_lifetime = BDM_BUNDLE_LIFETIME;

        if(strlen(cfg->resource_name_pattern) == 0){
            strncpy(cfg->resource_name_pattern, "*.*.*:*", 
                sizeof(cfg->resource_name_pattern));
        }

        // Set static bdmcfg used by parser and parse file
        bdmcfg = cfg;
        yyparse();
        bdmcfg = NULL;

        // Check for unset values 
        if(cfg->sync_recipient == NULL){
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
                "no sync recipient specified in config file '%s'\n",
                fname);

            if(cfg->sync_recipient){
                free(cfg->sync_recipient);
            }
            free(cfg);
            return NULL;
        }
    }

    return cfg;
    
}

void sync_sender_config_destroy(sync_sender_config_t *cfg) {
    if(cfg->sync_recipient) {
    	free(cfg->sync_recipient);
    }

    db_shutdown(cfg->db);
	
    free(cfg);
}


