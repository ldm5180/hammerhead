
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

bin_PROGRAMS = stream-manager

stream_manager_DEPENDENCIES = $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_CLIENT_QT4)

stream_manager_SOURCES = mainwindow.cxx node.cxx streamview.cxx main.cxx mainwindow.moc.cpp node.moc.cpp streamview.moc.cpp

BUILT_SOURCES = mainwindow.moc.cpp node.moc.cpp streamview.moc.cpp

stream_manager_CXXFLAGS = $(QT_CXXFLAGS) $(AM_CXXFLAGS) $(GLIB_CFLAGS) $(CFLAGS_BIONET_CLIENT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_CLIENT_QT4)

stream_manager_CPPFLAGS = $(QT_CPPFLAGS) $(AM_CPPFLAGS) $(GLIB_CFLAGS) $(CFLAGS_BIONET_CLIENT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_BIONET_CLIENT_QT4)

stream_manager_LDFLAGS = $(QT_LDFLAGS) $(AM_LDFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(LDFLAGS_BIONET_CLIENT) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_CAL_CLIENT) $(LDFLAGS_BIONET_CLIENT_QT4) $(LDFLAGS_QT)

stream_manager_LDADD = $(QT_LIBS) $(AM_LDADD) $(LIBS_BIONET_CLIENT) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_CAL_CLIENT) $(LIBS_BIONET_CLIENT_QT4) -lpthread -lm 

clean-local:
	rm -f $(BUILT_SOURCES)
