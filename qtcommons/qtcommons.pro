QT       += widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

TARGET = qtcommons
TEMPLATE = lib
DESTDIR=../

DEFINES += QTCOMMONS_LIBRARY

include ("$$top_srcdir/localconfig.pri")

SOURCES += qcustomplot.cpp \
        R/env.cpp

HEADERS +=\
        qtcommons_global.h \
        qcustomplot.h \
        R/env.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
