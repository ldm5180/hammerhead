
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

SUBDIRS = 

if ENABLE_CLIENTS
SUBDIRS += bdm-client bdm-watcher 
endif

if ENABLE_ION
SUBDIRS += bp-commander
endif

if ENABLE_PYTHON
SUBDIRS += py-libbdm bdmplot2 twisted bdmplot bdm-get bdm-watcher-py
endif
