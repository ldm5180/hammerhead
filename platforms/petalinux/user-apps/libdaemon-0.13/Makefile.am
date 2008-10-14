# This file is part of libdaemon.
#
# Copyright 2003-2008 Lennart Poettering
#
# libdaemon is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 2.1 of the
# License, or (at your option) any later version.
#
# libdaemon is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with libdaemon. If not, see
# <http://www.gnu.org/licenses/>.

EXTRA_DIST=bootstrap.sh LICENSE
SUBDIRS=libdaemon doc examples

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libdaemon.pc

noinst_DATA =
MAINTAINERCLEANFILES=

if USE_LYNX
noinst_DATA += README
EXTRA_DIST += README
MAINTAINERCLEANFILES += README

README:
	rm -f README
	$(MAKE) -C doc README
	cd $(srcdir) && ln -s doc/README README
endif

doxygen:
	$(MAKE) -C doc doxygen

homepage: dist doxygen
	test -d $$HOME/homepage/private
	mkdir -p $$HOME/homepage/private/projects/libdaemon
	cp libdaemon-@PACKAGE_VERSION@.tar.gz $$HOME/homepage/private/projects/libdaemon
	cp doc/README.html doc/style.css $$HOME/homepage/private/projects/libdaemon
	ln -sf README.html $$HOME/homepage/private/projects/libdaemon/index.html
	cp -av doc/reference/ $$HOME/homepage/private/projects/libdaemon/

.PHONY: homepage doxygen
