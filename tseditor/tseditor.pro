TEMPLATE=app
QT += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tsereditor

INCLUDEPATH+=../include/  ../commons/

include ("$$top_srcdir/localconfig.pri")

DESTDIR = ../

CONFIG(debug,release|debug) {
    DEFINES += DEBUG
}
CONFIG(release,release|debug) {
}

macx {
    CONFIG -= app_bundle
    #ICON = icons/dteditor.icns
}

win32 {
    RC_FILE = tseditor.rc
}


SOURCES += \
    tseditor.cpp \
    tseditorwindow.cpp

FORMS += \
    tseditorwindow.ui

HEADERS += \
    tseditorwindow.h

RESOURCES +=

OTHER_FILES += \
    tseditor.rc

TRANSLATIONS +=


target.path=$${PREFIX}/bin

