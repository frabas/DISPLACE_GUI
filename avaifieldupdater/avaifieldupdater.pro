include ("$$top_srcdir/localconfig.pri")

TEMPLATE = app
QT+= core
TARGET=avaifieldupdater
CONFIG += console c++11

DESTDIR = ../

DEFINES=
INCLUDEPATH=../include ../commons

macx {
    DESTDIR=$$EXEDESTDIR
    CONFIG -= app_bundle

    INCLUDEPATH += /usr/local/include
    LIBS += -L$$LIBDESTDIR
}
unix {
    DESTDIR=$$EXEDESTDIR
}

LIBS+=-L.. -lformats -ldisplacecommons

SOURCES += main.cpp
