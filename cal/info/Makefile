
include ../Makefile.params


MODULES = $(shell cat CAL-I-Modules-list)

SUBDIRS =       \
    $(MODULES)  \


all clean test:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done


install:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done
	install -m 0644 cal-info.h $(DESTDIR)/usr/include/

.PHONY: all test install clean

