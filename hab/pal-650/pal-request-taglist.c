
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "pal-650-hab.h"


int pal_request_taglist(int pal_fd) {
    int r;

    char *cmd = "taglist?\n";
    int size = strlen(cmd);

    
    r = write(pal_fd, cmd, size);
    if (r < 0) {
        g_warning("error writing %d-byte command '%s' to PAL: %s", 
			size, cmd, strerror(errno));

        return -1;
    }

    if (r != size) {
        g_warning("short write to PAL!");

        return -1;
    }

    return 0;
}

