
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

qtmake_moc_objects = moc_mainwindow.o moc_node.o moc_streamview.o

qtmake_headers = mainwindow.h node.h streamview.h

qtmake_sources = mainwindow.cxx node.cxx streamview.cxx main.cxx

qtmake_objects = mainwindow.o node.o streamview.o main.o



bin_PROGRAMS = stream-manager

stream_manager_DEPENDENCIES = makefile.qt $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_CLIENT_QT4)

stream_manager_SOURCES = makefile.qt 

stream_manager_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(CFLAGS_BIONET_CLIENT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_CLIENT_QT4) ${QT_CXXFLAGS} $(INCLUDES_QT)

stream_manager_LDFLAGS = $(AM_LDFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(LDFLAGS_BIONET_CLIENT) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_CAL_CLIENT) $(LDFLAGS_BIONET_CLIENT_QT4) $(LDFLAGS_QT)

stream_manager_LDADD = $(AM_LDADD) $(LIBS_BIONET_CLIENT) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_CLIENT) $(LIBS_BIONET_CLIENT_QT4) -lpthread -lm ${qtmake_moc_objects} ${qtmake_objects}



makefile.qt: $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_CLIENT_QT4) 
	${QT4_PATH} -Wall -o makefile.qt "INCLUDEPATH+=../libbionet ../../util ../libbionet-qt4 /usr/include" "HEADERS+=${qtmake_headers}" "LIBS+=${dummy_LDFLAGS} ${dummy_LDADD}" "SOURCES+=${qtmake_sources}" "PKGCONFIG+=glib-2.0" "TEMPLATE=app" "CONFIG+=link_pkgconfig debug"
	make -f makefile.qt mocables ${qtmake_moc_objects} ${qtmake_objects} 

clean-local:
	rm -f makefile.qt *.o moc*.cpp stream-manager
