include $(top_srcdir)/common.am

qtmake_moc_objects = moc_bionetio.o moc_bionetmodel.o moc_mainwindow.o moc_plotwindow.o moc_resourceview.o moc_samplehistory.o

qtmake_sources = bionetio.cxx bionetmodel.cxx mainwindow.cxx plotwindow.cxx resourceview.cxx samplehistory.cxx main.cxx

qtmake_objects = bionetio.o bionetmodel.o mainwindow.o plotwindow.o resourceview.o samplehistory.o main.o 



bin_PROGRAMS = bionet-monitor

bionet_monitor_DEPENDENCIES = makefile.qt ../libbionet/libbionet.la ../../util/libbionet-util.la ../../asn.1/libbionet-asn.la ../../cal/mdnssd-bip/client/libcal-mdnssd-bip-client.la 

bionet_monitor_SOURCES = makefile.qt 

bionet_monitor_CFLAGS = $(AM_CFLAGS) $(GLIB20_CFLAGS) -I../libbionet -I../../util -I/usr/include/qwt-qt4

bionet_monitor_LDFLAGS = $(AM_LDFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) -L../libbionet -L../../asn.1 -L../../util -L../../cal/mdnssd-bip/client

bionet_monitor_LDADD = $(AM_LDADD) -lbionet -lbionet-util -lbionet-asn -lcal-mdnssd-bip-client -lpthread -lm -lqwt-qt4 ${qtmake_moc_objects} ${qtmake_objects} 



makefile.qt:
	qmake-qt4 -Wall -o makefile.qt "INCLUDEPATH+=../libbionet ../../util /usr/include/qwt-qt4" "HEADERS+=bionetio.h bionetmodel.h mainwindow.h plotwindow.h resourceview.h samplehistory.h" "LIBS+=${dummy_LDFLAGS} ${dummy_LDADD}" "SOURCES+=${qtmake_sources}" "PKGCONFIG+=glib-2.0" "TEMPLATE=app" "QT+=network" "CONFIG+=link_pkgconfig debug"
	make -f makefile.qt mocables ${qtmake_moc_objects} ${qtmake_objects} 

clean-local:
	rm -f makefile.qt *.o moc*.cpp bionet-monitor
