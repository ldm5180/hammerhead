#include <glib.h>
#include <netinet/in.h>
#include "td-radio.h"

char *ip = NULL;

int port = 9400;
int timeout = 10;

double scan_data[80000];
short signed int complete_scan[COMPLETE_SCAN_SIZE];
long current_range_count = 0;
long scan_points = 0;

struct sockaddr_in local_address;
struct sockaddr_in radio_address;



