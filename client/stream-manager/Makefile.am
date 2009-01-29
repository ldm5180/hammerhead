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

qtmake_moc_objects = moc_mainwindow.o moc_node.o moc_stream.o moc_streamview.o

qtmake_headers = mainwindow.h node.h stream.h streamview.h

qtmake_sources = mainwindow.cxx node.cxx stream.cxx streamview.cxx main.cxx

qtmake_objects = mainwindow.o node.o stream.o streamview.o main.o



bin_PROGRAMS = stream-manager

stream_manager_DEPENDENCIES = makefile.qt $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL) 

stream_manager_SOURCES = makefile.qt 

stream_manager_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(CFLAGS_BIONET_CLIENT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_CLIENT_QT4) -I/usr/include/qwt-qt4 

stream_manager_LDFLAGS = $(AM_LDFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(LDFLAGS_BIONET_CLIENT) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_CAL_CLIENT) $(LDFLAGS_BIONET_QT4)  

stream_manager_LDADD = $(AM_LDADD) $(LIBS_BIONET_CLIENT) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_CLIENT) $(LIBS_BIONET_CLIENT_qt4) -lpthread -lm -lqwt-qt4 ${qtmake_moc_objects} ${qtmake_objects} 



makefile.qt: $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_CLIENT_QT4) 
	qmake-qt4 -Wall -o makefile.qt "INCLUDEPATH+=../libbionet ../../util /usr/include/qwt-qt4 ../libbionet-qt4" "HEADERS+=${qtmake_headers}" "LIBS+=${dummy_LDFLAGS} ${dummy_LDADD}" "SOURCES+=${qtmake_sources}" "PKGCONFIG+=glib-2.0" "TEMPLATE=app" "QT+=network" "CONFIG+=link_pkgconfig debug"
	make -f makefile.qt mocables ${qtmake_moc_objects} ${qtmake_objects} 

clean-local:
	rm -f makefile.qt *.o moc*.cpp stream-manager
