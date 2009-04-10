
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

SUBDIRS = libhab random csa-cp mmod syshealth parsec pal-650 uwb streamy

if ENABLE_ALSA
SUBDIRS += alsa
endif

if ENABLE_PYTHON
SUBDIRS += py-libhab
endif

if ENABLE_BLUETOOTH
SUBDIRS += libamedevice stethoscope cpod
endif

if HAVE_XSLTPROC
if ENABLE_LTKC
SUBDIRS += speedway
endif
endif
