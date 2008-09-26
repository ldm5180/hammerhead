
DIRS =     \
    cal    \



all clean:
	set -e; for i in $(DIRS); do $(MAKE) -C $$i $@; done


test:


.PHONY: all clean test

