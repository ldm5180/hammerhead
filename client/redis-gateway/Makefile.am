
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = bionet-redis-gateway


bionet_redis_gateway_SOURCES = redis-gateway.c

bionet_redis_gateway_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(CFLAGS_BIONET_CLIENT) \
	$(CFLAGS_BIONET_UTIL)

bionet_redis_gateway_LDFLAGS = \
	$(AM_LDFLAGS) \
	$(GLIB_LIBS) \
	$(LDFLAGS_BIONET_CLIENT) \
	$(LDFLAGS_BIONET_UTIL)

bionet_redis_gateway_LDADD = \
	$(AM_LDADD) \
	$(LIBS_BIONET_CLIENT) \
	$(LIBS_BIONET_UTIL) \
	-lhiredis \
	-lpthread \
	-lm 

