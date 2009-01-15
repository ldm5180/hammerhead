#ifndef __TD_RADIO_H_
#define __TD_RADIO_H_

#include <stdbool.h>

#define NO_SCAN			0
#define WRONG_SEGMENT	1
#define NEXT_SEGMENT	2
#define SCAN_COMPLETE	3

#define SCAN_SIZE			2500
#define COMPLETE_SCAN_SIZE	32767
#define MAX_SAVE_SCANS		1000
#define NO_LOCK				1

// Todo: why was this size chosen ?
#define BUFFER_SZ			32767 //20000

extern double scan_data[];
extern short signed int complete_scan[];
extern long current_range_count;
extern long scan_points;

extern char *ip;
extern char *nag_hostname;

extern int port;
extern int timeout;

extern struct sockaddr_in local_address;
extern struct sockaddr_in radio_address;

int radio_connect(const char *radio_ip, const int radio_port);
int radio_send(int fd);
int radio_read(int fd, char* buffer);

int process_data(char *buffer);
int do_output();
void print_buffer();

// If the TD-P210 had any actuators we wanted to expose, this callback would
// let us know when something wanted to change them.
void cb_set_resource(const char *node_id, const char *resource_id, 
	const char *value);

void make_shutdowns_clean(void);

#endif 

