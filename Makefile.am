
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


#to do test coverage analysis using gcov, uncomment
#CFLAGS = -g -O0 -Wall -W -fprofile-arcs -ftest-coverage
#CXXFLAGS=-g -O0 -Wall -W -Wshadow -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers -Wno-deprecated -Woverloaded-virtual -Wwrite-strings -fprofile-arcs -ftest-coverage
#LDFLAGS=-fprofile-arcs -ftest-coverage -lgcov

include $(top_srcdir)/common.am

ACLOCAL_AMFLAGS = -I build-aux/m4

SUBDIRS = cal util asn.1 client hab data-manager security check

if ENABLE_LINUX
LDCONFIG_EXE = /sbin/ldconfig
endif

if ENABLE_DOXYGEN
docdir = doxygen
doc_DATA = doxygen/html/index.html
DOXYGEN_INPUT = $(top_srcdir)/client/libbionet/bionet.h $(top_srcdir)/hab/libhab/hardware-abstractor.h $(top_srcdir)/util/bionet-hab.h $(top_srcdir)/util/bionet-node.h $(top_srcdir)/util/bionet-resource.h $(top_srcdir)/util/bionet-stream.h $(top_srcdir)/util/bionet-util.h
DOXYFILE = Doxyfile
endif

if DISABLE_DOT
DOXYFILE = Doxyfile.nodot
endif

all-local: $(DOXYGEN_INPUT) $(doc_DATA) 

doxygen/html/index.html: $(DOXYGEN_INPUT)
	doxygen $(DOXYFILE)

clean-local:
	rm -rf doxygen

install-exec-hook:
	$(LDCONFIG_EXE)

.PHONY: test
test:
	(cd test; ./do-test)

release: all-local version.h

delversion:
	rm -f version.h

version.h:
	bzr version-info --custom --template="#define VERSION_INFO \"Bionet {branch_nick} {revision_id}\"\n\n#define SHORT_VERSION_INFO \"Bionet {branch_nick} r{revno}\"\n" > version.h
