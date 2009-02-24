
# Copyright (c) 2008-2009, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


TEMPLATE = app
TARGET += 
DEPENDPATH += .

INC_DIR = /tmp/include
LIB_DIR = /tmp/lib

libs.path = $$LIB_DIR
libs.files = bionet-qt4.a
INSTALLS += libs

headers.path = $$INC_DIR
headers.files = $$HEADERS
INSTALLS += headers

