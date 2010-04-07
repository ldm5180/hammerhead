
#
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.
#


include ../Makefile.params


all: announcer listener


announcer: $(foreach M,$(CAL_PD_MODULES),$M-announcer)

%-announcer: announcer.o ${CAL_DIR}/peer-discovery/%/cal-pd.a
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS) $($(patsubst %-announcer,%,$@)_LDLIBS)


listener: $(foreach M,$(CAL_PD_MODULES),$M-listener)

%-listener: listener.o ${CAL_DIR}/peer-discovery/%/cal-pd.a
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS) $($(patsubst %-listener,%,$@)_LDLIBS)


clean:
	set -e; for i in $(CAL_PD_MODULES); do   \
	    rm -f $$i-announcer $$i-listener;    \
	done

	rm -f recording.*.std???

	rm -f announcer.o listener.o


.PHONY: all announcer listener clean


