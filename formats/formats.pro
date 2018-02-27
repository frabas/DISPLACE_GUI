TEMPLATE= lib
TARGET=formats
DESTDIR=../
CONFIG += c++14

include ("$$top_srcdir/localconfig.pri")
macx:DESTDIR=$$LIBDESTDIR

DEFINES += FORMATS_LIBRARY
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
    utils/vectorsdata.h \
    formatexception.h \
    utils/MultifieldWriter.h \
    legacy/binarygraphfilereader.h \
    utils/endian.h \
    legacy/binarygraphfilewriter.h \
    utils/portable_endian.h \
    formats_globals.h

SOURCES = legacy/ConfigFileReader.cpp  \
    utils/KeyValueReader.cpp \
    utils/LineNumberReader.cpp

OTHER_FILES = Makefile.hpc

win32 {
    LIBS +=
}

!win32 {
    LIBS +=
}

target.path = $${PREFIX}/lib
headers.path = $${PREFIX}/include/
headers.files = $${HEADERS}

INSTALLS += target

