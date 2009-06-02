
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

SUBDIRS = libhab random csa-cp syshealth pal-650 streamy test-pattern

if ENABLE_ALSA
SUBDIRS += alsa
endif

if ENABLE_PYTHON
SUBDIRS += py-libhab
endif

if ENABLE_BLUETOOTH
SUBDIRS += libamedevice stethoscope cpod
endif

if ENABLE_MMOD
SUBDIRS += mmod
endif

if ENABLE_PARSEC
SUBDIRS += parsec
endif

if ENABLE_UWB
SUBDIRS += uwb
endif

if HAVE_XSLTPROC
if ENABLE_LTKC
SUBDIRS += speedway
endif
endif
