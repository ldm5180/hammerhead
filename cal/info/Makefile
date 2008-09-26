
include ../Makefile.params


MODULES =     \
    bip       \

SUBDIRS =       \
    $(MODULES)  \

#    shared      \
#    examples    \


all:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i all; done

clean:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

.PHONY: all clean

