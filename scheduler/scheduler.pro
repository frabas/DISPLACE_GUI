TEMPLATE = app
QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = scheduler

INCLUDEPATH += ../include/ ../commons ../qtcommons ../formats/

include ("$$top_srcdir/localconfig.pri")

win32 {
    DESTDIR = ..
}
!win32 {
    DESTDIR = ../bin
}

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
    rundialog.cpp \
    batchcreatedialog.cpp \
    windowsscriptgenerator.cpp

HEADERS  += mainwindow.h \
    schedulerjob.h \
    simulationrun.h \
    schedulerjobadapter.h \
    rundialog.h \
    batchcreatedialog.h \
    windowsscriptgenerator.h

FORMS    += mainwindow.ui \
    rundialog.ui \
    batchcreatedialog.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    scheduler.rc \
    resources/templates/windows-runqueue.txt


target.path=$${PREFIX}/bin
INSTALLS += target
