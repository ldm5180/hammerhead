include $(top_srcdir)/common.am

SUBDIRS = libhab random csa-cp mmod syshealth parsec speedway pal-650 uwb

if ENABLE_ALSA
SUBDIRS += alsa
endif

if ENABLE_PYTHON
SUBDIRS += py-libhab
endif

if ENABLE_BLUETOOTH
SUBDIRS += libamedevice stethoscope cpod
endif
