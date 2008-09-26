
include ../Makefile.params


MODULES =     \
    bip       \

SUBDIRS =       \
    $(MODULES)  \

#    shared      \
#    examples    \


all clean test install:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

.PHONY: all test install clean

