TEMPLATE = app
QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = scheduler

INCLUDEPATH += ../include/ ../commons ../qtcommons ../formats/

include ("$$top_srcdir/localconfig.pri")

DESTDIR = ../bin

macx {
    DESTDIR=$$EXEDESTDIR
    CONFIG -= app_bundle
}


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


target.path=$${PREFIX}/bin
INSTALLS += target
