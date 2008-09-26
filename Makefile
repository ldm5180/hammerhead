
DIRS =     \
    cal    \



all test install clean:
	set -e; for i in $(DIRS); do $(MAKE) -C $$i $@; done


.PHONY: all install clean test

