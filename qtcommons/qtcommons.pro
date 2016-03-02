QT       += widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

CONFIG += c++11

TARGET = qtcommons
TEMPLATE = lib
DESTDIR=../

DEFINES += QTCOMMONS_LIBRARY

include ("$$top_srcdir/localconfig.pri")
macx:DESTDIR=$$LIBDESTDIR

SOURCES += qcustomplot.cpp \
        R/env.cpp \
        csv/csvimporter.cpp \
        csv/csvexporter.cpp \
        csv/csvtablemodel.cpp

HEADERS +=\
        qtcommons_global.h \
        qcustomplot.h \
        R/env.h \
        csv/csvimporter.h \
        csv/csvexporter.h \
        csv/csvtablemodel.h \
    qsignalblocker.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
