
//
// Copyright (C) 2004-2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bionet.h"




void usage(void) {
    fprintf(stderr, "usage: bionet-stream OPTIONS [STREAM]\n\
\n\
If STREAM is specified, it connects to the named stream.  If STREAM is not\n\
specified, it prints a continuously updating list of the available streams.\n\
\n\
OPTIONS:\n\
\n\
    --help  Show this help.\n\
\n\
");
}
    



void cb_lost_node(bionet_node_t *node) {
    int i;

    if (node->streams) {
        g_log("", G_LOG_LEVEL_INFO, "lost node: %s.%s.%s", node->hab->type, node->hab->id, node->id);

        for (i = 0; i < bionet_node_get_num_streams(node); i ++) {
            bionet_stream_t *stream = bionet_node_get_stream_by_index(node, i);
            g_log(
                "", G_LOG_LEVEL_INFO,
                "    %s %s %s", 
                stream->id,
                stream->type,
                bionet_stream_direction_to_string(stream->direction)
            );
        }
    }
}


void cb_new_node(bionet_node_t *node) {
    int i;

    if (node->streams) {
        g_log("", G_LOG_LEVEL_INFO, "new node: %s.%s.%s", node->hab->type, node->hab->id, node->id);

        for (i = 0; i < bionet_node_get_num_streams(node); i ++) {
            bionet_stream_t *stream = bionet_node_get_stream_by_index(node, i);
            g_log(
                "", G_LOG_LEVEL_INFO,
                "    %s %s %s", 
                stream->id,
                stream->type,
                bionet_stream_direction_to_string(stream->direction)
            );
        }
    }
}




void list_streams(void) {
    int bionet_fd;


    g_log_set_default_handler(bionet_glib_log_handler, NULL);


    // this must happen before anything else
    bionet_fd = bionet_connect();
    if (bionet_fd < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "error connecting to Bionet");
        exit(1);
    }
    g_log("", G_LOG_LEVEL_INFO, "connected to Bionet");


    bionet_register_callback_new_node(cb_new_node);
    bionet_register_callback_lost_node(cb_lost_node);

    bionet_subscribe_node_list_by_name("*.*.*");


    while (1) {
        int r;
        fd_set readers;

        FD_ZERO(&readers);
        FD_SET(bionet_fd, &readers);

        r = select(bionet_fd + 1, &readers, NULL, NULL, NULL);

        if ((r < 0) && (errno != EINTR)) {
            g_log("", G_LOG_LEVEL_WARNING, "error from select: %s", strerror(errno));
            g_usleep(1000*1000);
            continue;
        }

        bionet_read();
    } 
}




void read_from_stream(bionet_stream_t *stream) {
    int fd;

    fprintf(stderr, "reading from %s\n", stream->id);

    // unbuffer stdout
    setvbuf(stdout, NULL, _IONBF, 0);

    fd = *(int*)stream->user_data;

    while (1) {
        int index;
        int bytes_remaining;
        int r;
        uint8_t buffer[1024];

        r = read(fd, buffer, sizeof(buffer));
        if (r < 0) {
            fprintf(stderr, "error reading stream: %s\n", strerror(errno));
            exit(1);
        }

        if (r == 0) {
            fprintf(stderr, "end of file\n");
            exit(0);
        }

        // fprintf(stderr, "read %d bytes from stream\n", r);

        index = 0;
        bytes_remaining = r;

        while (bytes_remaining > 0) {
            r = write(fileno(stdout), &buffer[index], bytes_remaining);
            if (r < 0) {
                fprintf(stderr, "error writing to stdout: %s\n", strerror(errno));
                exit(1);
            }

            index += r;
            bytes_remaining -= r;
        }
    }
}




void write_to_stream(bionet_stream_t *stream) {
    int fd;

    fprintf(stderr, "writing to %s\n", stream->id);
    
    // unbuffer stdin
    setvbuf(stdin, NULL, _IONBF, 0);

    fd = *(int*)stream->user_data;

    while (1) {
        int index;
        int bytes_remaining;
        int r;
        uint8_t buffer[1024];

        r = read(fileno(stdin), buffer, sizeof(buffer));
        if (r < 0) {
            fprintf(stderr, "error reading stdin: %s\n", strerror(errno));
            exit(1);
        }

        if (r == 0) {
            fprintf(stderr, "end of file\n");
            exit(0);
        }

        index = 0;
        bytes_remaining = r;

        // fprintf(stderr, "writing %d bytes to stream\n", r);

        while (bytes_remaining > 0) {
            r = write(fd, &buffer[index], bytes_remaining);
            if (r < 0) {
                fprintf(stderr, "error writing to stream: %s\n", strerror(errno));
                exit(1);
            }

            index += r;
            bytes_remaining -= r;
        }
    }
}




void connect_to_stream(const char *stream_name) {
#if 0
    int r;
    GSList *nodes;

    char *node_name;
    char *stream_id;

    bionet_stream_t *stream;


    if (stream_name == NULL) {
        fprintf(stderr, "NULL stream name passed to connect_to_stream()\n");
        exit(1);
    }

    node_name = strdup(stream_name);
    if (node_name == NULL) {
        fprintf(stderr, "out of memory in connect_to_stream()\n");
        exit(1);
    }

    stream_id = strchr(node_name, ':');
    if (stream_id == NULL) {
        fprintf(stderr, "stream name '%s' has no ':'\n", stream_name);
        exit(1);
    }

    *stream_id = (char)0;
    stream_id ++;

    r = bionet_list_nodes_by_name_pattern(&nodes, node_name);
    if (r < 0) {
        fprintf(stderr, "error getting node %s\n", node_name);
        exit(1);
    }

    if (nodes == NULL) {
        fprintf(stderr, "node %s not found\n", node_name);
        exit(1);
    }

    stream = bionet_node_get_stream_by_id(nodes->data, stream_id);
    if (stream == NULL) {
        fprintf(stderr, "node %s has no stream %s\n", node_name, stream_id);
        exit(1);
    }

    free(node_name);

    r = bionet_stream_connect(stream);
    if (r < 0) {
        fprintf(stderr, "error connecting to stream %s\n", stream_name);
        exit(1);
    }
    stream->user_data = (int *)malloc(sizeof(int));
    if (stream->user_data == NULL) {
        fprintf(stderr, "out of memory!\n");
        exit(1);
    }
    *(int*)stream->user_data = r;

    if (stream->direction == BIONET_STREAM_DIRECTION_PRODUCER) {
        read_from_stream(stream);
    } else {
        write_to_stream(stream);
    }
#endif
}




int main(int argc, char *argv[]) {
    int i;
    char *stream_name = NULL;

    for (i = 1; argv[i] != NULL; i ++) {

        if (strcmp(argv[i], "--help") == 0) {
            usage();
            exit(0);

        } else {
            if (i != (argc - 1)) {
                // multiple arguments!
                fprintf(stderr, "unknown argument '%s'\n", argv[i]);
                usage();
                exit(1);
            }

            stream_name = argv[i];
        }
    }


    if (stream_name == NULL) {
        list_streams();
        exit(0);
    }


    connect_to_stream(stream_name);

    exit(0);
}
