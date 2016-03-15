CONFIG -= qt

TEMPLATE= lib
TARGET=formats
DESTDIR=../
CONFIG += c++11

include ("$$top_srcdir/localconfig.pri")
macx:DESTDIR=$$LIBDESTDIR

INCLUDEPATH += .

win32 {
}

unix:!macx {
}

HEADERS = \
    legacy/ConfigFileReader.h \
    utils/KeyValueReader.h \
    utils/LineNumberReader.h \
    utils/prettyprint.h \
    legacy/NodesFileReader.h utils/MultifieldReader.h \
    utils/vectorsdata.h

SOURCES = legacy/ConfigFileReader.cpp  \
    utils/KeyValueReader.cpp \
    utils/LineNumberReader.cpp

LIBS += -lboost_filesystem -lboost_regex
