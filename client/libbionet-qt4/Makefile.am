#
# Copyright (C) 2004-2008, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of GNU General Public License version 2, as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
#

include $(top_srcdir)/common.am

qtmake_moc_objects = moc_bionetio.o moc_bionetmodel.o

qtmake_headers = bionetio.h bionetmodel.h

qtmake_sources = bionetio.cxx bionetmodel.cxx

qtmake_objects = bionetio.o bionetmodel.o



lib_LTLIBRARIES = libbionet-qt4.la

libbionet_qt4_la_DEPENDENCIES = makefile.qt $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_ASN1) $(INCLUDES_BIONET_CAL_CLIENT)

libbionet_qt4_la_SOURCES = makefile.qt 

libbionet_qt4_la_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(CFLAGS_BIONET_CLIENT) $(CFLAGS_BIONET_UTIL)

libbionet_qt4_la_LDFLAGS = $(AM_LDFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(LDFLAGS_BIONET_CLIENT) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_CAL_CLIENT) -version-info $(BIONET_INTERFACE_VERSION) 

libbionet_qt4_la_LIBADD = $(AM_LDADD) -lpthread -lm ${qtmake_moc_objects} ${qtmake_objects} $(LIBS_BIONET_CLIENT) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_CLIENT)


makefile.qt:
	qmake-qt4 -Wall -o makefile.qt "INCLUDEPATH+=../libbionet ../../util /usr/include/" "HEADERS+=${qtmake_headers}" "LIBS+=${dummy_LDFLAGS} ${dummy_LDADD}" "SOURCES+=${qtmake_sources}" "PKGCONFIG+=glib-2.0" "TEMPLATE=lib" "QT+=network" "CONFIG+=link_pkgconfig debug"
	make -f makefile.qt mocables ${qtmake_moc_objects} ${qtmake_objects} 

clean-local:
	rm -f makefile.qt *.o moc*.cpp
