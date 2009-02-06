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

