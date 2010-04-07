
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>



//  Function for setting up syslog while being a daemon

void daemon_log_handler(
    const gchar *log_domain,
    GLogLevelFlags log_level,
    const gchar *message,
    gpointer verbosity
    )
{
    if (*(int*)verbosity  == 1 ||
        log_level <= G_LOG_LEVEL_INFO)
    {
        if ((log_domain == NULL) || (log_domain[0] == '\0')) {
            syslog(LOG_INFO, "%s", message);
        } else {
            syslog(LOG_INFO, "%s: %s\n", log_domain, message);
        }
    }
}



//  Lifted from libutil that we can't include...

int daemonize(int *verbosity)
{
    int r;

    r = chdir("/");
    if (r == -1) {
        g_log("", G_LOG_LEVEL_CRITICAL, "Failed to chdir to '/': %s", strerror(errno));
        return -1;
    }

    fflush(NULL);

    r = fork();
    if (r > 0) exit(0);    // parent
    if (r < 0) {
        // failed to fork
        g_log("", G_LOG_LEVEL_CRITICAL, "Failed to fork: %s", strerror(errno));
        return -1;
    }

    fclose(stdin);
    fclose(stdout);
    fclose(stderr);

    openlog("mmod", 0, 0);
    g_log_set_default_handler(daemon_log_handler, verbosity);
    g_log("", G_LOG_LEVEL_INFO, "starting up...");

    return 0;
}
