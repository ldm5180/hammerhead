#
# Makefile to install iphone-dev tools
# 

all:



ifndef DESTDIR
DESTDIR=dist_root
endif

ifndef prefix
prefix=@PREFIX@
endif

ARCHS=arm-apple-darwin i686-apple-darwin

TOOLS=ar g++ gcc ld nm ranlib

install:
	mkdir -p ${DESTDIR}/${prefix}/bin
	cp xcode-iphone-tool-wrapper.sh ${DESTDIR}/${prefix}/bin/.
	cp port-iphone ${DESTDIR}/${prefix}/bin/.
	cp port-iphone-sim ${DESTDIR}/${prefix}/bin/.
	set -e; \
	cd ${DESTDIR}/${prefix}/bin; \
	for arch in ${ARCHS}; do \
	  for tool in ${TOOLS}; do \
	    ln -s xcode-iphone-tool-wrapper.sh $${arch}-$${tool}; \
	  done; \
	done;
	

	
	

	
