
DIRS =     \
    cal    \



all: doc
	set -e; for i in $(DIRS); do $(MAKE) -C $$i $@; done

doc:
	doxygen

test:
	set -e; for i in $(DIRS); do $(MAKE) -C $$i $@; done

install: doc
	set -e; for i in $(DIRS); do $(MAKE) -C $$i $@; done
	install -m 0755 -d $(DESTDIR)/usr/share/doc/bionet2/html
	install -m 0644 html/* $(DESTDIR)/usr/share/doc/bionet2/html

clean:
	set -e; for i in $(DIRS); do $(MAKE) -C $$i $@; done
	rm -rf html


.PHONY: all doc install clean test

