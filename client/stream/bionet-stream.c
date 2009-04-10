
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "bionet.h"

int bionet_fd;




void usage(void) {
    fprintf(stderr, 
	    "'bionet-stream' connects to Bionet streams.\n"
	    "\n"							  
	    "Usage: bionet-stream [OPTIONS] [STREAM]\n"
	    "\n"
	    " -h,--help            Show this help\n"
	    " -v,--version         Show version number\n"
	    "\n"
	    "If STREAM is specified, it connects to the named stream.  For Producer\n"
	    "Streams, the stream data is printed to stdout.  For Consumer Streams, the\n"
	    "data is read from stdin.\n"
	    "\n"
	    "If STREAM is not specified, it prints a continuously updating list of the\n"
	    "available streams.\n");
}
    
void version(void) {
    fprintf(stdout, "No version info available.\n");
}


void cb_lost_node(bionet_node_t *node) {
    int num_streams;
    int i;

    num_streams = bionet_node_get_num_streams(node);
    if (num_streams <= 0) return;

    fprintf(stderr, "lost node: %s\n", bionet_node_get_name(node));

    for (i = 0; i <num_streams; i ++) {
        bionet_stream_t *stream = bionet_node_get_stream_by_index(node, i);
        fprintf(
            stderr,
            "    %s %s %s\n", 
            bionet_stream_get_id(stream),
            bionet_stream_get_type(stream),
            bionet_stream_direction_to_string(bionet_stream_get_direction(stream))
        );
    }
}


void cb_new_node(bionet_node_t *node) {
    int num_streams;
    int i;

    num_streams = bionet_node_get_num_streams(node);
    if (num_streams <= 0) return;

    fprintf(stderr, "new node: %s\n", bionet_node_get_name(node));

    for (i = 0; i < num_streams; i ++) {
        bionet_stream_t *stream = bionet_node_get_stream_by_index(node, i);
        fprintf(
            stderr,
            "    %s %s %s\n", 
            bionet_stream_get_id(stream),
            bionet_stream_get_type(stream),
            bionet_stream_direction_to_string(bionet_stream_get_direction(stream))
        );
    }
}


void cb_stream(bionet_stream_t *stream, void *buffer, int size) {
    int r;

    r = write(fileno(stdout), buffer, size);
    if (r < 0) {
        fprintf(stderr, "error writing to stdout: %s\n", strerror(errno));
        exit(1);
    }
    if (r < size) {
        fprintf(stderr, "short write to stdout: %s\n", strerror(errno));
        exit(1);
    }
}




// we're just going to list all the Bionet streams as they come and go
void list_streams(void) {
    bionet_register_callback_new_node(cb_new_node);
    bionet_register_callback_lost_node(cb_lost_node);

    bionet_subscribe_node_list_by_name("*.*.*");

    while (1) {
        bionet_read_with_timeout(NULL);
    }
}




void read_from_stream(bionet_stream_t *stream) {
    fprintf(stderr, "reading from %s\n", bionet_stream_get_name(stream));

    // close stdin
    fclose(stdin);

    // unbuffer stdout
    setvbuf(stdout, NULL, _IONBF, 0);

    bionet_register_callback_stream(cb_stream);

    bionet_subscribe_stream_by_name(bionet_stream_get_name(stream));

    while (1) {
        bionet_read_with_timeout(NULL);
    }
}




void write_to_stream(bionet_stream_t *stream) {
    fprintf(stderr, "writing to %s\n", bionet_stream_get_name(stream));

    // unbuffer stdin
    setvbuf(stdin, NULL, _IONBF, 0);

    while (1) {
        int r;
        uint8_t buffer[1024];

        r = read(fileno(stdin), buffer, sizeof(buffer));
        if (r < 0) {
            fprintf(stderr, "error reading stdin: %s\n", strerror(errno));
            exit(1);
        }
        if (r == 0) {
            fprintf(stderr, "end of file\n");
            sleep(2);  // FIXME: to let bionet send out the data...
            exit(0);
        }

        bionet_stream_write(stream, buffer, r);
    }
}




void deal_with_stream(bionet_stream_t *stream) {
    switch (bionet_stream_get_direction(stream)) {
        case BIONET_STREAM_DIRECTION_PRODUCER: {
            read_from_stream(stream);
            fprintf(stderr, "read_from_stream() returned!\n");
            exit(1);
        }

        case BIONET_STREAM_DIRECTION_CONSUMER: {
            write_to_stream(stream);
            fprintf(stderr, "write_to_stream() returned!\n");
            exit(1);
        }

        default: {
            fprintf(stderr, "stream %s has unknown direction!\n", bionet_stream_get_name(stream));
            exit(1);
        }
    }
}




int main(int argc, char *argv[]) {
    //int i;
    int r;
    char *stream_name = NULL;

    char *hab_type;
    char *hab_id;
    char *node_id;
    char *stream_id;

    int c;

    g_log_set_default_handler(bionet_glib_log_handler, NULL);

    while(1) {
	int i = 0;
	static struct option long_options[] = {
	    {"help", 0, 0, 'h'},
	    {"version", 0, 0, 'v'},
	    {0, 0, 0, 0} //this must be last in the list
	};

	c = getopt_long(argc, argv, "hv?", long_options, &i);
	if (c == -1) {
	    break;
	}

	switch (c) {
	case 'h':
	case '?':
	    usage();
	    return 0;

	case 'v':
	    version();
	    return 0;

    	default:
	    fprintf(stderr, "Incorrect usage.\n\n");
	    usage();
	    return -1;
	}
    }

    if (argc > 1) {
	stream_name = argv[argc-1];
    }

    bionet_fd = bionet_connect();
    if (bionet_fd < 0) {
        fprintf(stderr, "error connecting to Bionet\n");
        exit(1);
    }
    fprintf(stderr, "connected to Bionet\n");

    // if no stream name was specified, we just list all the streams as they come and goo
    if (stream_name == NULL) list_streams();

    // if we get here, we're going to connect to a stream
    r = bionet_split_resource_name(stream_name, &hab_type, &hab_id, &node_id, &stream_id);
    if (r != 0) exit(1);

    // we need to subscribe to the node-list so we discover the Stream, so we can learn if it's a Producer or Consumer
    {
        char node_name[(3 * BIONET_NAME_COMPONENT_MAX_LEN) + 3];
        int r;

        r = snprintf(node_name, sizeof(node_name), "%s.%s.%s", hab_type, hab_id, node_id);
        if ((r >= sizeof(node_name)) || (r < 5)) {
            fprintf(stderr, "error making node name for node-list subscription\n");
            exit(1);
        }

        bionet_subscribe_node_list_by_name(node_name);
    }


    while (1) {
        bionet_stream_t *stream;

        bionet_read_with_timeout(NULL);

        stream = bionet_cache_lookup_stream(hab_type, hab_id, node_id, stream_id);
        if (stream == NULL) continue;

        deal_with_stream(stream);
        // wont return
    } 

    exit(0);
}

