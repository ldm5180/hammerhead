
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = bdm-client

bdm_client_SOURCES = bdm-client.c                

bdm_client_CFLAGS = $(AM_CFLAGS) $(CFLAGS_BDM) -fno-strict-aliasing

bdm_client_LDFLAGS = $(AM_LDFLAGS) $(LDFLAGS_BDM)

bdm_client_LDADD = $(AM_LDADD) $(LIBS_BDM) 
