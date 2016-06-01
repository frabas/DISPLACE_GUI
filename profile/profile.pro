include ("$$top_srcdir/localconfig.pri")

QT=
TARGET=unitprofile
CONFIG += c++11

DESTDIR = ../

DEFINES=
INCLUDEPATH=../include ../commons
LIBS=-L.. -lformats -ldisplacecommons -lpthread -lrt

QMAKE_CXXFLAGS += -pg
QMAKE_LFLAGS += -pg -static-libgcc

SOURCES= main.cpp \
    rutils.cpp

HEADERS=

OTHER_FILES +=

