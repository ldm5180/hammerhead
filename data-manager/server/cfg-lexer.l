
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>


#include "bionet-data-manager.h"
#include "cfg-parser.h"

int line_num = 0;

%}

%option nounput
%option noinput

%%


method                 { return SET_METHOD; }
start_time             { return SET_START; }
stop_time              { return SET_STOP; }
resource_name_pattern  { return SET_RES; }
frequency              { return SET_FREQ; }
sync_recipient         { return SET_RECPT; }
sync_port              { return SET_PORT; }
bundle_lifetime        { return SET_BUNDLE_LIFETIME; }
tcp                    { return METH_TCP; }
ion                    { return METH_ION; }


[0-9]{4}-[0-9]{1,2}-[0-9]{1,2}\ [0-9]{1,2}:[0-9]{1,2}:[0-9]{1,2}(\.[0-9]+)?(\ [+-][0-9]{4})? {
    struct tm tgm = { 0 };
    char * tstr = strdup(yytext);
    char *s;
    char *ss = tstr;

    yylval.timeval.tv_sec = 0;
    yylval.timeval.tv_usec = 0;

    s = strsep(&ss, " -:");
    tgm.tm_year = strtoul(s, NULL, 10) - 1900;

    s = strsep(&ss, " -:");
    tgm.tm_mon = strtoul(s, NULL, 10) -1;

    s = strsep(&ss, " -:");
    tgm.tm_mday = strtoul(s, NULL, 10);

    s = strsep(&ss, " -:");
    tgm.tm_hour = strtoul(s, NULL, 10);

    s = strsep(&ss, " -:");
    tgm.tm_min = strtoul(s, NULL, 10);

    s = strsep(&ss, " -:");
    char * usec;
    if((usec = strchr(s, '.'))) {
        double d = strtod(usec, NULL);
        yylval.timeval.tv_usec = d * 1e6;
        usec[0] = '\0';
    }
    tgm.tm_sec = strtoul(s, NULL, 10);

    s = strsep(&ss, " ");
    int gmtoff = 0;
    if(s){
        gmtoff = (
              (s[4] - '0') * 60
            + (s[3] - '0') * 600
            + (s[2] - '0') * 3600
            + (s[1] - '0') * 36000) 
        * (s[0] == '+'?1:-1);
    }

    yylval.timeval.tv_sec = timegm(&tgm);
    yylval.timeval.tv_sec += gmtoff;
    free(tstr);

    return TIMEVAL;
}


[+-]?[0-9]{1,10} {
    yylval.intval = strtol(yytext, NULL, 10);
    return INTVAL;
}

[ \t]*=[ \t]*                   { return ASSIGN; }

[^ \n\t=]([^\n=]*[^\ \n\t=])?     { yylval.stringval = strdup(yytext);
                                  return STRINGVAL;
                                }

[ \t]+$                          ;/* Ignore trailing whitespace */

[ \t]*                           { return OPTWHITE;}


^[^ \n\t=]+                     { yylval.stringval = strdup(yytext);
                                  return SET_UNKNOWN; 
                                }

^[ \t]*(#.*)?\n                 {line_num++; /* Ignore Comments; empty lines */}
\n                              {line_num++;}


%%

int yywrap() { return 1; }

int yyerror() {
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "error at line %d (%s)\n", yylineno, yytext);
    return 0;
}
