
include ../Makefile.params

LDLIBS = -lpthread `pkg-config --libs glib-2.0`

ifeq (${TARGET_OS},LINUX)
dnssd_LDLIBS = `pkg-config --libs avahi-compat-libdns_sd`
endif


MODULES =     \
    dnssd     \
    bdp       \

SUBDIRS =       \
    shared      \
    $(MODULES)  \
    examples    \


all test install clean:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

.PHONY: all test install clean

