
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <wchar.h>

#include "internal.h"
#include "bionet-util.h"

#ifdef _WIN32
#include <winsock2.h> // needed for gethostname
#include <winbase.h> // needed for GetModuleFileName
#endif

static char *hab_get_program_name(void);
static int bionet_hab_set_type(bionet_hab_t *hab, const char *type);
static int bionet_hab_set_id(bionet_hab_t *hab, const char *id);


const char * bionet_hab_get_type(const bionet_hab_t *hab) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_hab_get_type(): NULL HAB passed in!");
        return NULL;
    }

    return hab->type;
}


const char * bionet_hab_get_id(const bionet_hab_t *hab) {
    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_hab_get_type(): NULL HAB passed in!");
        return NULL;
    }

    return hab->id;
}

static int bionet_hab_set_type(bionet_hab_t *hab, const char *type) {

    // 
    // sanity checks
    //

    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_set_type(): NULL HAB passed in!");
        return -1;
    }

    if ((type != NULL) && !bionet_is_valid_name_component(type)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_set_type(): invalid HAB-Type passed in!");
        return -1;
    }

    if (hab->type != NULL) {
        free(hab->type);
        hab->type = NULL;
    }

    if (type != NULL) {
        hab->type = strdup(type);
        if (hab->type == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_hab_set_type(): out of memory!");
            return -1;
        }
    }

    if (NULL == type)
    {
	char * tmp_type;
        tmp_type = hab_get_program_name();
        if (tmp_type == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_hab_set_type(): the passed-in HAB has no HAB-Type, and cannot get program name");
            return -1;
        }

	hab->type = strdup(tmp_type);
        if (hab->type == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_hab_set_type(): out of memory!");
            return -1;
        }

        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	      "bionet_hab_set_type(): the passed-in HAB has no HAB-Type, using program name '%s'", 
	      hab->type);
    }

    return 0;
}


static int bionet_hab_set_id(bionet_hab_t *hab, const char *id) {

    // 
    // sanity checks
    //

    if (hab == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_set_id(): NULL HAB passed in!");
        return -1;
    }

    if ((id != NULL) && !bionet_is_valid_name_component(id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_hab_set_id(): invalid HAB-ID passed in!");
        return -1;
    }


    if (hab->id != NULL) {
        free(hab->id);
        hab->id = NULL;
    }

    if (id != NULL) {
        hab->id = strdup(id);
        if (hab->id == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_hab_set_id(): out of memory!");
            return -1;
        }
    }

    if (NULL == id)
    {
        char hostname[256];
        char *p;
        int r;

        r = gethostname(hostname, sizeof(hostname));
        if (r < 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_hab_set_id(): the passed-in HAB has no HAB-ID, and could not get hostname: %s", 
		  strerror(errno));
            return -1;
        }

        for (p = hostname; *p != '\0'; p++) {
            if (!isalnum(*p) && (*p != '-')) *p = '-';
        }

	hab->id = strdup(hostname);
        if (hab->id == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_hab_set_id(): out of memory!");
            return -1;
        }
    }

    return 0;
}


bionet_hab_t* bionet_hab_new(const char* type, const char* id) {

    bionet_hab_t* hab;

    hab = calloc(1, sizeof(bionet_hab_t));
    if (hab == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_hab_new(): out of memory!");
	return NULL;
    }

    if (bionet_hab_set_type(hab, type) != 0) {
        // an error has been logged
        bionet_hab_free(hab);
        return NULL;
    }

    if (bionet_hab_set_id(hab, id) != 0) {
        // an error has been logged
        bionet_hab_free(hab);
        return NULL;
    }

    return hab;
}

static char *hab_get_program_name(void) {
    static char program_name[500];
#if defined(LINUX) || defined(MACOSX)

    int r;
    int fd;
    char *tmp;

    fd = open("/proc/self/cmdline", O_RDONLY);
    if (fd < 0) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "hab_get_program_name(): error opening /proc/self/cmdline (%s), oh well",
            strerror(errno)
        );
        return "unknown";
    }


    r = read(fd, program_name, sizeof(program_name) - 1);
    close(fd);

    if (r <= 0) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "hab_connect_to_nag(): error reading /proc/self/cmdline (%s), oh well",
            strerror(errno)
        );
        return "unknown";
    }

    program_name[r] = '\0';  // this is redundant, because the /proc file already contains the NUL, but it makes Coverity happy

    while ((tmp = memchr(program_name, '/', strlen(program_name))) != NULL) {
        int new_len = strlen(tmp+1);
        memmove(program_name, tmp+1, new_len);
        program_name[new_len] = '\0';
    }

    for (tmp = program_name; *tmp != '\0'; tmp ++) {
        if (!isalnum(*tmp) && *tmp != '-') {
            *tmp = '-';
        }
    }

#endif

#ifdef _WIN32
    wchar_t * tmp;
    static const size_t nchars = 500;
    wchar_t wstr[nchars*2];
    
    GetModuleFileNameW(NULL, wstr, nchars);

    while ((tmp = wmemchr(wstr, L'\\', wcslen(wstr))) != NULL) {
        int new_len = wcslen(tmp+1);
        memmove(wstr, tmp+1, new_len);
        wstr[new_len] = L'\0';
    }

    for (tmp = wstr; *tmp != L'\0'; tmp ++) {
        if (!iswalnum(*tmp) && *tmp != '-') {
            *tmp = L'-';
        }
    }

    if(wcstombs(program_name, wstr, sizeof(program_name)) < 0 ) {
        strcpy(program_name, "unknown");
    }

#endif
    return program_name;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
