HEADERS += cgbaSim.h \
    cgbaDial.h \
    qled.h \
    cgbasimmainwindow.h \
    cgbaled.h \
    dial.h
SOURCES += main.cpp \
    cgbaSim.cpp \
    cgbaDial.cpp \
    qled.cpp \
    cgbasimmainwindow.cpp \
    cgbaled.cpp \
    dial.cpp
BIONET = ../..
INCLUDEPATH += $${BIONET}/client/libbionet-qt4 \
    $${BIONET}/libbionet \
    $${BIONET}/util
LIBS += -L$${BIONET}/client/libbionet-qt4/.lib \
    -L$${BIONET}/libbionet/.lib \
    -L$${BIONET}/util/.lib \
    -L$${BIONET}/libbionet \
    -L$${BIONET}/util \
    -L$${BIONET}/client/libbionet-qt4
LIBS += -lbionet \
    -lbionet-util \
    -lbionet-qt4
CONFIG += link_pkgconfig \
    debug
PKGCONFIG += glib-2.0
QT += svg
