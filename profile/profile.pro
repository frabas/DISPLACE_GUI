include ("$$top_srcdir/localconfig.pri")

QT=
TARGET=unitprofile
CONFIG += c++11

DESTDIR = ../

DEFINES=
INCLUDEPATH=../include ../commons

macx {
    DESTDIR=$$EXEDESTDIR
    CONFIG -= app_bundle

    INCLUDEPATH += /usr/local/include
    LIBS += -L$$LIBDESTDIR
}

LIBS+=-L.. -lformats -ldisplacecommons

!macx{
    LIBS += -lpthread -lrt
}

CONFIG(gprof) {
    QMAKE_CXXFLAGS += -pg
    QMAKE_LFLAGS += -pg -static-libgcc
}

SOURCES= main.cpp \
    rutils.cpp

HEADERS=

OTHER_FILES +=

