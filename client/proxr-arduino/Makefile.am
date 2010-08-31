
include $(top_srcdir)/common.am

include $(top_srcdir)/build-aux/autotroll.mk

bin_PROGRAMS = proxr-arduino-client

proxr_arduino_client_DEPENDENCIES = $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_CLIENT_QT4) 

proxr_arduino_client_SOURCES =  \
    cgbaDial.cpp		        \
    cgbaled.cpp	                \
    cgbaSim.cpp                 \
    cgbasimmainwindow.cpp       \
    dial.cpp                    \
    main.cpp                    \
    qled.cpp			        \
    ini-read.c                  \
    cgbaDial.moc.cpp	    	\
    cgbaled.moc.cpp	            \
    cgbaSim.moc.cpp             \
    cgbasimmainwindow.moc.cpp   \
    dial.moc.cpp                \
    qled.moc.cpp		        \
    icons.qrc.cpp

BUILT_SOURCES =  	            \
    cgbaDial.moc.cpp		    \
    cgbaled.moc.cpp	            \
    cgbaSim.moc.cpp             \
    cgbasimmainwindow.moc.cpp   \
    dial.moc.cpp                \
    qled.moc.cpp		        \
    icons.qrc.cpp	 		

proxr_arduino_client_CPPFLAGS = -g $(QT_CPPFLAGS) $(AM_CPPFLAGS) $(GLIB_CFLAGS) $(CFLAGS_BIONET_CLIENT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_QWT) $(CFLAGS_BIONET_CLIENT_QT4) $(CFLAGS_BDM)

proxr_arduino_client_CFLAGS = -g $(GLIB_CFLAGS)

proxr_arduino_client_LDFLAGS = $(QT_LDFLAGS) $(AM_LDFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(LDFLAGS_BIONET_CLIENT) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_CAL_CLIENT) $(LDFLAGS_BIONET_CLIENT_QT4) $(LDFLAGS_BDM) $(GLIB_LIBS)

proxr_arduino_client_LDADD = $(QT_LIBS) $(AM_LDADD) -lpthread -lm $(QWT_LIBADD) $(LIBS_BIONET_CLIENT) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_CAL_CLIENT) $(LIBS_BIONET_CLIENT_QT4) $(LIBS_BDM)

clean-local:
	rm -f *.o proxr-arduino-client $(BUILT_SOURCES)
