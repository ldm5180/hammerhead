
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am
SUBDIRS = libbionet watcher stream

if ENABLE_QT4
SUBDIRS += libbionet-qt4 monitor stream-manager
endif

if ENABLE_PYTHON
SUBDIRS += py-libbionet
endif

