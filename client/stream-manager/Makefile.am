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

qtmake_sources = mainwindow.cxx node.cxx stream.cxx streamview.cxx

qtmake_objects = mainwindow.o node.o stream.o streamview.o main.o



bin_PROGRAMS = stream-manager

stream_manager_DEPENDENCIES = makefile.qt ../libbionet/libbionet.la ../../util/libbionet-util.la ../../asn.1/libbionet-asn.la ../../cal/mdnssd-bip/client/libcal-mdnssd-bip-client.la ../libbionet-qt4/libbionet-qt4.la

stream_manager_SOURCES = makefile.qt 

stream_manager_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libbionet -I../../util -I/usr/include/qwt-qt4 -I../libbionet-qt4 

stream_manager_LDFLAGS = $(AM_LDFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) -L../libbionet -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/client -L../libbionet-qt4 

stream_manager_LDADD = $(AM_LDADD) -lbionet-qt4 -lbionet -lbionet-util -lbionet-asn -lcal-mdnssd-bip-client -lpthread -lm -lqwt-qt4 ${qtmake_moc_objects} ${qtmake_objects} 



makefile.qt: ../libbionet/libbionet.la ../../util/libbionet-util.la ../../asn.1/libbionet-asn.la ../../cal/mdnssd-bip/client/libcal-mdnssd-bip-client.la ../libbionet-qt4/libbionet-qt4.la
	qmake-qt4 -Wall -o makefile.qt "INCLUDEPATH+=../libbionet ../../util /usr/include/qwt-qt4 ../libbionet-qt4" "HEADERS+=${qtmake_headers}" "LIBS+=${dummy_LDFLAGS} ${dummy_LDADD}" "SOURCES+=${qtmake_sources}" "PKGCONFIG+=glib-2.0" "TEMPLATE=app" "QT+=network" "CONFIG+=link_pkgconfig debug"
	make -f makefile.qt mocables ${qtmake_moc_objects} ${qtmake_objects} 

clean-local:
	rm -f makefile.qt *.o moc*.cpp stream-manager
