
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

SUBDIRS = 

if ENABLE_QT4
SUBDIRS += libbionet-qt4
endif

if ENABLE_PYTHON
SUBDIRS += py-libbionet twisted plot
endif

if ENABLE_CLIENTS
SUBDIRS += realtime-watcher stream commander get

if ENABLE_QT4
SUBDIRS += monitor stream-manager
endif

if ENABLE_BDM
SUBDIRS += watcher
endif

endif
