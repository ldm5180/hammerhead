
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
If STREAM is specified, it connects to the named stream.  For Producer\n\
Streams, the stream data is printed to stdout.  For Consumer Streams, the\n\
data is read from stdin.\n\
\n\
If STREAM is not specified, it prints a continuously updating list of the\n\
available streams.\n\
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




void read_from_stream(bionet_stream_t *stream) {
    fprintf(
        stderr,
        "reading from %s.%s.%s:%s\n",
        stream->node->hab->type,
        stream->node->hab->id,
        stream->node->id,
        stream->id
    );

    pause();

#if 0
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
#endif
}




void write_to_stream(bionet_stream_t *stream) {
    fprintf(
        stderr,
        "writing to %s.%s.%s:%s\n",
        stream->node->hab->type,
        stream->node->hab->id,
        stream->node->id,
        stream->id
    );

    pause();

#if 0
    int fd;

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
#endif
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
            fprintf(
                stderr,
                "stream %s.%s.%s:%s has unknown direction!\n",
                stream->node->hab->type,
                stream->node->hab->id,
                stream->node->id,
                stream->id
            );
            exit(1);
        }
    }


#if 0
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
    int bionet_fd;

    char *hab_type;
    char *hab_id;
    char *node_id;
    char *stream_id;


    g_log_set_default_handler(bionet_glib_log_handler, NULL);


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


    bionet_fd = bionet_connect();
    if (bionet_fd < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "error connecting to Bionet");
        exit(1);
    }
    g_log("", G_LOG_LEVEL_INFO, "connected to Bionet");


    if (stream_name == NULL) {
        bionet_register_callback_new_node(cb_new_node);
        bionet_register_callback_lost_node(cb_lost_node);
    } else {
        int r;

        r = bionet_split_resource_name(stream_name, &hab_type, &hab_id, &node_id, &stream_id);
        if (r != 0) {
            g_log("", G_LOG_LEVEL_WARNING, "error splitting stream name '%s'", stream_name);
            exit(1);
        }
    }


    bionet_subscribe_node_list_by_name("*.*.*");


    while (1) {
        int r;
        fd_set readers;
        bionet_stream_t *stream;

        FD_ZERO(&readers);
        FD_SET(bionet_fd, &readers);

        r = select(bionet_fd + 1, &readers, NULL, NULL, NULL);

        if ((r < 0) && (errno != EINTR)) {
            g_log("", G_LOG_LEVEL_WARNING, "error from select: %s", strerror(errno));
            g_usleep(1000*1000);
            continue;
        }

        bionet_read();

        if (stream_name == NULL) continue;

        stream = bionet_cache_lookup_stream(hab_type, hab_id, node_id, stream_id);
        if (stream == NULL) continue;

        printf("found stream %s!\n", stream_name);
        deal_with_stream(stream);
        // wont return
    } 

    exit(0);
}

