#
# Developer makefile. Do not use unless you know what you are doing
#
#
# This makfile contains several targets that simplify hte process of modifying
# the iphone and iphone-simulator macports environments setup from this 
# directory

MACPORTS_VERS=1.7.0

IPHONEOS_SYSROOT=/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS3.0.sdk
IPHONESIM_SYSROOT=/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator2.2.sdk

all:
	echo "Please specify a target to build"
	false

install: install-macports-iphone ports-install

#
# Incomplete uninstaller
#
uninstall:
	rm -rf /opt/local/i686-apple-darwin
	rm -rf /Library/Tcl/i686-apple-darwin
	rm -rf /opt/local/arm-apple-darwin
	rm -rf /Library/Tcl/arm-apple-darwin
	rm -rf /opt/local/iPhoneOS3.0
	rm -rf /Library/Tcl/iPhoneOS3.0

# Create two parallel directories, one original and one modified for
# editing. When done editing, re-make the patchfile...
ports-trees:
	rm -rf ports
	rm -rf ports.orig
	tar xzf macports-ports.tgz
	mv ports ports.orig
	tar xzf macports-ports.tgz
	(cd ports && patch -p1) < macports-ports.patch


# Generate a patch file after making modifications to the 'ports' directory.
# Make sure there aren't any transient files first...
.PHONY: macports-ports.patch
macports-ports.patch: 
	[ -d ports ] && [ -d ports.orig ]
	diff -ruN ports.orig ports > macports-ports.patch || true


# Create two parallel directories, one original and one modified for
# editing. When done editing, re-make the patchfile...
macports-trees:
	rm -rf MacPorts-$(MACPORTS_VERS)
	rm -rf MacPorts-$(MACPORTS_VERS).orig
	tar xjf MacPorts-$(MACPORTS_VERS).tar.bz2
	mv MacPorts-$(MACPORTS_VERS) MacPorts-$(MACPORTS_VERS).orig
	tar xjf MacPorts-$(MACPORTS_VERS).tar.bz2
	(cd MacPorts-$(MACPORTS_VERS) && patch -p1) < MacPorts-$(MACPORTS_VERS).patch
	touch MacPorts-$(MACPORTS_VERS)


# Generate a patch file after making modifications to MacPorts
# Make sure there aren't any transient files first...

#.PHONY : MacPorts-$(MACPORTS_VERS)-arm-apple-darwin.patch
#.PHONY : MacPorts-$(MACPORTS_VERS)-i686-apple-darwin.patch
#.PHONY : MacPorts-$(MACPORTS_VERS)-*.patch

MacPorts-$(MACPORTS_VERS).patch: MacPorts-$(MACPORTS_VERS) MacPorts-$(MACPORTS_VERS).orig
	[ -d MacPorts-$(MACPORTS_VERS) ] && [ -d MacPorts-$(MACPORTS_VERS).orig ]
	diff -ruN MacPorts-$(MACPORTS_VERS).orig MacPorts-$(MACPORTS_VERS) > $@ || true
	sleep 1
	touch $<


#
# Install both versions of macports
#
install-macports-iphone: install-macports-iPhoneOS3.0 install-macports-iPhoneSimulator3.0

#
# Install macports for iphone device
#
install-macports-arm-apple-darwin: SYSROOT=/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS2.2.sdk
install-macports-arm-apple-darwin: SYSARCH=arm

install-macports-iPhoneOS3.0: SYSROOT=$(IPHONEOS_SYSROOT)
install-macports-iPhoneOS3.0: SYSARCH=arm

#
# Install macports for iphone simulator
#
install-macports-i696-apple-darwin: SYSROOT=/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator2.2.sdk
install-macports-i696-apple-darwin: SYSARCH=i686

install-macports-iPhoneSimulator3.0: SYSROOT=/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator3.0.sdk
install-macports-iPhoneSimulator3.0: SYSARCH=i686


#	rm -rf tmp_macports
#	mkdir tmp_macports
#	(cd tmp_macports && tar xjf ../MacPorts-$(MACPORTS_VERS).tar.bz2)
#	(cd tmp_macports/MacPorts-$(MACPORTS_VERS) && patch -p1) \
#		< MacPorts-$(MACPORTS_VERS).patch
#	(cd tmp_macports/MacPorts-$(MACPORTS_VERS) && \
#		./configure --prefix=/opt/local/arm-apple-darwin \
#		      --with-tclpackage=/Library/Tcl/arm-apple-darwin)
#	(cd tmp_macports/MacPorts-$(MACPORTS_VERS) && make)
#	@echo "Using sudo to install macports into /opt/local/arm-apple-darwin"
#	(cd tmp_macports/MacPorts-$(MACPORTS_VERS) && sudo make install)
#	rm -rf tmp_macports
#


# 
# Install macports into the architecture specified by %
# and the SDK specified by SYSROOT
#
# NOT AND EXTERNAL TARGET
#
install-macports-%:
	#@echo "macports install $* ${SYSROOT}"
	rm -rf tmp_macports
	mkdir tmp_macports
	(cd tmp_macports && tar xjf ../MacPorts-$(MACPORTS_VERS).tar.bz2)
	(cd tmp_macports/MacPorts-$(MACPORTS_VERS) && patch -p1) \
		< MacPorts-$(MACPORTS_VERS).patch
	(cd tmp_macports/MacPorts-$(MACPORTS_VERS) && \
		./configure --prefix=/opt/local/$* \
		      --with-universal-target="10.5" \
		      --with-universal-sysroot="${SYSROOT}" \
		      --with-universal-archs=${SYSARCH} \
		      --with-tclpackage=/Library/Tcl/$*)
	(cd tmp_macports/MacPorts-$(MACPORTS_VERS) && make)
	@echo "Using sudo to install macports into /opt/local/$*"
	(cd tmp_macports/MacPorts-$(MACPORTS_VERS) && sudo make install)
	sudo /opt/local/$*/bin/port sync
	#rm -rf tmp_macports


#
# Install all ports overlays
#
ports-install:
	rm -rf tmp_ports
	mkdir -p tmp_ports
	(cd tmp_ports && tar xzf ../macports-ports.tgz)
	(cd tmp_ports/ports && patch -p1) < macports-ports.patch
	mv tmp_ports/ports tmp_ports/iphone
	mkdir -p tmp_ports/base
	cp -r extra-ports/* tmp_ports/base/.
	@echo "Using sudo to install ports into /usr/local/ports"
	sudo rm -rf /usr/local/ports
	sudo mkdir /usr/local/ports
	sudo cp -r tmp_ports/{base,iphone} /usr/local/ports/.
	(cd /usr/local/ports/base && sudo portindex)
	(cd /usr/local/ports/iphone && sudo portindex)
	rm -rf tmp_ports
	
fix-sdk-link:
	sh ./fix-sdk-links.sh
	
#
# vim: nosta noexpandtab sw=4 ts=4
#
