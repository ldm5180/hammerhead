
include ../Makefile.params

LDLIBS = -lpthread `pkg-config --libs glib-2.0`

ifeq (${TARGET_OS},LINUX)
dnssd_LDLIBS = `pkg-config --libs avahi-compat-libdns_sd`
endif


MODULES = $(shell cat CAL-PD-Modules-list)


SUBDIRS =       \
    $(MODULES)  \
    examples    \


all test clean:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done


install:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done
	install -m 0644 cal-peer-discovery.h $(DESTDIR)/usr/include/


.PHONY: all test install clean

