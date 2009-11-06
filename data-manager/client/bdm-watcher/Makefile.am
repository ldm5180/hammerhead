
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = bdm-watcher

bdm_watcher_SOURCES = bdm-watcher.c                

bdm_watcher_CFLAGS = $(AM_CFLAGS) $(CFLAGS_BDM) -fno-strict-aliasing

bdm_watcher_LDFLAGS = $(AM_LDFLAGS) $(LDFLAGS_BDM)

bdm_watcher_LDADD = $(AM_LDADD) $(LIBS_BDM) 
