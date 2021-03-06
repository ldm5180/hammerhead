
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.

include $(top_srcdir)/common.am

ACLOCAL_AMFLAGS = -I build-aux/m4

SUBDIRS = cal util asn.1 libhab libbionet

if ENABLE_BDM
if ENABLE_ION
SUBDIRS += bps
endif
SUBDIRS += libbdm data-manager
endif

SUBDIRS += hab client security apps check test

if ENABLE_LINUX
LDCONFIG_EXE = /sbin/ldconfig
endif

if ENABLE_DOXYGEN
doc_DATA = doc/quickstart.html

DOXYGEN_INPUT = $(top_srcdir)/libbionet/bionet.h $(top_srcdir)/libhab/hardware-abstractor.h $(top_srcdir)/libbdm/bdm-client.h $(top_srcdir)/util/bionet-bdm.h $(top_srcdir)/util/bionet-hab.h $(top_srcdir)/util/bionet-node.h $(top_srcdir)/util/bionet-resource.h $(top_srcdir)/util/bionet-stream.h $(top_srcdir)/util/bionet-util.h $(top_srcdir)/mainpage.h $(top_srcdir)/bps/bps_socket.h
DOXYFILE = Doxyfile

install-data-local: 
	$(MKDIR_P) $(DESTDIR)$(docdir)
	for f in doxygen/html/*; do \
	   $(INSTALL_DATA) $(srcdir)/$$f $(DESTDIR)$(docdir)/.; \
	done 

endif 
#end ENABLE_DOXYGEN


all-local: $(DOXYGEN_INPUT) doxygen/html/index.html

doxygen/html/index.html: $(DOXYGEN_INPUT) $(DOXYFILE)
	doxygen $(DOXYFILE)

$(DOXYGEN_FILES): doxygen/html/index.html

clean-local:
	rm -rf doxygen
	(cd test; ./clean-test)

if ENABLE_WIN32
# Make a MS Visual C package....
install-exec-hook:
	cd $(DESTDIR)$(libdir) && \
	  for f in *.dll.a; do \
	    mv -f $$f `echo $$f | sed -e 's/.dll.a$$/.lib/'`; \
	  done && \
	  for f in *.a; do \
	    mv -f $$f `echo $$f | sed -e 's/.a$$/_static.lib/'`; \
	  done;

else
install-exec-hook:
	-$(LDCONFIG_EXE)
endif


.PHONY: test
test:
	(cd test; ./do-test)




release: all-local version.h

delversion:
	rm -f version.h

version.h:
	bzr version-info --custom --template="#define VERSION_INFO \"Bionet {branch_nick} {revision_id}\"\n\n#define SHORT_VERSION_INFO \"Bionet {branch_nick} r{revno}\"\n" > version.h
