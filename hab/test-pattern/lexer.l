
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>


#include "bionet-util.h"
#include "test-pattern-hab.h"
#include "parser.h"

int line_num = 0;

%}

%option nounput
%option noinput

%%

\+                                      { return ADD; }
\-                                      { return REMOVE; }
\?                                      { return NOVALUE; }
'[^'\n]+'                               { yylval.string_v = strdup(yytext); return VALUE; }
[Ss][Ee][Nn][Ss][Oo][Rr]                { yylval.flavor_v = BIONET_RESOURCE_FLAVOR_SENSOR; return FLAVOR; }
[Pp][Aa][Rr][Aa][Mm][Ee][Tt][Ee][Rr]    { yylval.flavor_v = BIONET_RESOURCE_FLAVOR_PARAMETER; return FLAVOR; }
[Aa][Cc][Tt][Uu][Aa][Tt][Oo][Rr]        { yylval.flavor_v = BIONET_RESOURCE_FLAVOR_ACTUATOR; return FLAVOR; }
[0-9]{4}-[0-9]{2}-[0-9]{2}[ ][0-9]{2}:[0-9]{2}:[0-9]{2}\.[0-9]{0,6}  { 
                                             yylval.tv_v = calloc(1, sizeof(struct timeval));
                                             str_to_timeval(yytext, yylval.tv_v);
                                             return TIMEVAL; 
                                        }
[Uu]?[Ii][Nn][Tt](8|16|32)              { yylval.datatype_v = bionet_resource_data_type_from_string(yytext); return DATATYPE; }
[Ff][Ll][Oo][Aa][Tt]                    { yylval.datatype_v = BIONET_RESOURCE_DATA_TYPE_FLOAT; return DATATYPE; }
[Dd][Oo][Uu][Bb][Ll][Ee]                { yylval.datatype_v = BIONET_RESOURCE_DATA_TYPE_DOUBLE; return DATATYPE; }
[Ss][Tt][Rr][Ii][Nn][Gg]                { yylval.datatype_v = BIONET_RESOURCE_DATA_TYPE_STRING; return DATATYPE; }
[Bb][Ii][Nn][Aa][Rr][Yy]                { yylval.datatype_v = BIONET_RESOURCE_DATA_TYPE_BINARY; return DATATYPE; }
[-a-zA-Z0-9/._:?*]+                     { yylval.string_v = strdup(yytext); return NAME; }
\n                                      { line_num ++; }
[ \t]                                   { }

%%

int yywrap() { return 1; }

int yyerror() {
    g_log("", G_LOG_LEVEL_ERROR, "error at line %d (%s)\n", line_num, yytext);
    return 0;
}
