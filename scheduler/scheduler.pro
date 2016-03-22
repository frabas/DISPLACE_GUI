#-------------------------------------------------
#
# Project created by QtCreator 2016-03-15T17:20:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = scheduler
TEMPLATE = app
CONFIG += c++11

INCLUDEPATH += ../include/ ../commons ../qtcommons ../formats/

include ("$$top_srcdir/localconfig.pri")

DESTDIR = ../bin

CONFIG(debug,release|debug) {
    DEFINES += DEBUG
}

win32 {
    RC_FILE = scheduler.rc
}

SOURCES += main.cpp\
        mainwindow.cpp \
    schedulerjob.cpp \
    simulationrun.cpp \
    schedulerjobadapter.cpp \
    rundialog.cpp

HEADERS  += mainwindow.h \
    schedulerjob.h \
    simulationrun.h \
    schedulerjobadapter.h \
    rundialog.h

FORMS    += mainwindow.ui \
    rundialog.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    scheduler.rc
