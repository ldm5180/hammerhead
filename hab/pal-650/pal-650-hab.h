
#ifndef __PAL_650_HAB_H
#define __PAL_650_HAB_H


#include <glib.h>


#define Max(a, b)  ((a) > (b) ? (a) : (b))


extern GHashTable *nodes;

extern char *nag_hostname;
extern char *pal_ip;
extern int pal_port;
extern int tag_timeout;


int pal_connect(const char *pal_ip, const int pal_port);
int pal_read(int pal_fd);
int pal_request_taglist(int pal_fd);


void timeout_tags(void);
void drop_node(gpointer node);


// If the PAL-650 had any actuators we wanted to expose, this callback would 
// let us know when someone wanted to change them.
void cb_set_resource(const char *node_id, const char *resource_id, const char *value);


void make_shutdowns_clean(void);


#endif 

