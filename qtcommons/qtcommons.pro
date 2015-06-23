QT       += widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

TARGET = qtcommons
TEMPLATE = lib
DESTDIR=../

DEFINES += QTCOMMONS_LIBRARY

include ("$$top_srcdir/localconfig.pri")

SOURCES += qcustomplot.cpp

HEADERS +=\
        qtcommons_global.h \
        qcustomplot.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
