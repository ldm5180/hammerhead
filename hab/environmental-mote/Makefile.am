
# Copyright (c) 2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

if TINYOSMIG_DISABLED
else
BUILT_SOURCES = envgenmsg.c envgenmsg.h 

envgenmsg.c: env-node/env_msg.h
	touch envgenmsg.c
	mig -o envgenmsg.h -c-prefix=ENVGENMSG c env-node/env_msg.h env_general_msg

envgenmsg.h: env-node/env_msg.h
	touch envgenmsg.c
	mig -o envgenmsg.h -c-prefix=ENVGENMSG c env-node/env_msg.h env_general_msg

clean-local:
	rm -f envgenmsg.*
endif

bin_PROGRAMS = env-hab

env_hab_DEPENDENCIES = $(BUILT_SOURCES) $(INCLUDES_BIONET_HAB) $(INCLUDES_BIONET_UTIL) 

env_hab_SOURCES =    \
    daemonize.c       \
    gateway.c         \
    main.c            \
    message.c         \
    envgenmsg.c      \
    env-message.c    \
    mts310_cook.c     \
    serialsource.c

env_hab_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(CFLAGS_BIONET_HAB) \
	$(CFLAGS_BIONET_UTIL) 

env_hab_LDFLAGS = \
	$(AM_LDFLAGS) \
	$(GLIB_LIBS) \
	$(AVAHI_COMPAT_LIBDNS_SD_LIBS) \
	$(DBUS_LIBS) \
	$(LDFLAGS_BIONET_HAB) \
	$(LDFLAGS_BIONET_UTIL) \
	$(LDFLAGS_BIONET_ASN1) \
	$(LDFLAGS_BIONET_CAL_SERVER)

env_hab_LDADD = \
	$(AM_LDADD) \
	$(LIBS_BIONET_HAB) \
	$(LIBS_BIONET_UTIL) \
	$(LIBS_BIONET_ASN1) \
	$(LIBS_BIONET_CAL_SERVER) \
	-lpthread \
	-lm



