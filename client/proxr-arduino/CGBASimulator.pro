HEADERS += cgbaSim.h \
    cgbaDial.h \
    qled.h \
    cgbasimmainwindow.h \
    cgbaled.h \
    dial.h
TEMPLATE = app
TARGET =
QT += svg
CONFIG += link_pkgconfig \
    debug
PKGCONFIG += glib-2.0
