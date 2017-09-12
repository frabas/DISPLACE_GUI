include ("$$top_srcdir/localconfig.pri")

TEMPLATE = app
QT+= core
TARGET=vmsmerger
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

LIBS+=-L.. -lformats -ldisplacecommons

SOURCES += main.cpp
