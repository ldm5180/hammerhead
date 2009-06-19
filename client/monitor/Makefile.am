
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

qtmake_moc_objects = moc_mainwindow.o moc_scaleinfo.o moc_plotwindow.o moc_plotpreferences.o moc_resourceview.o moc_history.o moc_archive.o

qtmake_headers = mainwindow.h scaleinfo.h plotpreferences.h plotwindow.h resourceview.h history.h archive.h

qtmake_sources = mainwindow.cxx scaleinfo.cxx plotpreferences.cxx plotwindow.cxx resourceview.cxx history.cxx archive.cxx main.cxx

qtmake_objects = mainwindow.o scaleinfo.o plotpreferences.o plotwindow.o resourceview.o history.o archive.o main.o 



bin_PROGRAMS = bionet-monitor

bionet_monitor_DEPENDENCIES = makefile.qt $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_CLIENT_QT4) 

bionet_monitor_SOURCES = makefile.qt 

bionet_monitor_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) $(CFLAGS_BIONET_CLIENT) $(CFLAGS_BIONET_UTIL) -I/usr/include/qwt-qt4 $(CFLAGS_BIONET_CLIENT_QT4) 

bionet_monitor_LDFLAGS = $(AM_LDFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(LDFLAGS_BIONET_CLIENT) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_CAL_CLIENT) $(LDFLAGS_BIONET_CLIENT_QT4)


bionet_monitor_LDADD = $(AM_LDADD) -lpthread -lm -lqwt-qt4 ${qtmake_moc_objects} ${qtmake_objects} $(LIBS_BIONET_CLIENT) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_CAL_CLIENT) $(LIBS_BIONET_CLIENT_QT4)



makefile.qt: $(LIBS_BIONET_CLIENT) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_CAL_CLIENT) $(LIBS_BIONET_CLIENT_QT4)
	${QT4_PATH} -Wall -o makefile.qt "INCLUDEPATH+=../libbionet ../../util /usr/include/qwt-qt4 ../libbionet-qt4" "HEADERS+=${qtmake_headers}" "LIBS+=${dummy_LDFLAGS} ${dummy_LDADD}" "SOURCES+=${qtmake_sources}" "PKGCONFIG+=glib-2.0" "TEMPLATE=app" "QT+=network" "CONFIG+=link_pkgconfig debug" 
	+make -f makefile.qt mocables ${qtmake_moc_objects} ${qtmake_objects} 

clean-local:
	rm -f makefile.qt *.o moc*.cpp bionet-monitor
