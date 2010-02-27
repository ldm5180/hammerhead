
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am

include $(top_srcdir)/build-aux/autotroll.mk


bin_PROGRAMS = bionet-monitor

bionet_monitor_DEPENDENCIES = $(INCLUDES_BIONET_CLIENT) $(INCLUDES_BIONET_UTIL) $(INCLUDES_BIONET_CLIENT_QT4) 

bionet_monitor_SOURCES =        \
    mainwindow.cxx              \
    monitorpage.cxx             \
    bionetpage.cxx              \
    scaleinfo.cxx               \
    plotpreferences.cxx         \
    plotwindow.cxx              \
    subscontroller.cxx          \
    bdmio.cxx                   \
    bdmconnectiondialog.cxx     \
    resourceview.cxx            \
    history.cxx                 \
    archive.cxx                 \
    main.cxx                    \
    mainwindow.moc.cpp          \
    monitorpage.moc.cpp         \
    bionetpage.moc.cpp          \
    scaleinfo.moc.cpp           \
    plotwindow.moc.cpp          \
    plotpreferences.moc.cpp     \
    subscontroller.moc.cpp      \
    bdmio.moc.cpp               \
    bdmconnectiondialog.moc.cpp \
    resourceview.moc.cpp        \
    history.moc.cpp             \
    archive.moc.cpp

BUILT_SOURCES =                 \
    mainwindow.moc.cpp          \
    monitorpage.moc.cpp         \
    bionetpage.moc.cpp          \
    scaleinfo.moc.cpp           \
    plotwindow.moc.cpp          \
    plotpreferences.moc.cpp     \
    subscontroller.moc.cpp      \
    bdmio.moc.cpp               \
    bdmconnectiondialog.moc.cpp \
    resourceview.moc.cpp        \
    history.moc.cpp             \
    archive.moc.cpp

bionet_monitor_CXXFLAGS = $(QT_CXXFLAGS) $(AM_CFLAGS) $(GLIB_CFLAGS) $(CFLAGS_BIONET_CLIENT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_QWT) $(CFLAGS_BIONET_CLIENT_QT4) 

bionet_monitor_CPPFLAGS = $(QT_CPPFLAGS) $(AM_CPPFLAGS) $(GLIB_CFLAGS) $(CFLAGS_BIONET_CLIENT) $(CFLAGS_BIONET_UTIL) $(CFLAGS_QWT) $(CFLAGS_BIONET_CLIENT_QT4) $(CFLAGS_BDM)

bionet_monitor_LDFLAGS = $(QT_LDFLAGS) $(AM_LDFLAGS) $(AVAHI_COMPAT_LIBDNS_SD_LIBS) $(LDFLAGS_BIONET_CLIENT) $(LDFLAGS_BIONET_ASN1) $(LDFLAGS_BIONET_UTIL) $(LDFLAGS_BIONET_CAL_CLIENT) $(LDFLAGS_BIONET_CLIENT_QT4) $(LDFLAGS_BDM)


bionet_monitor_LDADD = $(QT_LIBS) $(AM_LDADD) -lpthread -lm $(QWT_LIBADD) $(LIBS_BIONET_CLIENT) $(LIBS_BIONET_ASN1) $(LIBS_BIONET_UTIL) $(LIBS_BIONET_CAL_CLIENT) $(LIBS_BIONET_CLIENT_QT4) $(LIBS_BDM)

clean-local:
	rm -f $(BUILT_SOURCES)
