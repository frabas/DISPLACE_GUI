TEMPLATE=app
QT += core gui printsupport concurrent
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tsereditor

INCLUDEPATH+=../include/  ../commons/ ../qtcommons

include ("$$top_srcdir/localconfig.pri")

DESTDIR = ../
macx {
    DESTDIR=$$EXEDESTDIR
    CONFIG -= app_bundle
}

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

LIBS += -L.. -lformats -lqtcommons

SOURCES += \
    tseditor.cpp \
    tseditorwindow.cpp

FORMS += \
    tseditorwindow.ui

HEADERS += \
    tseditorwindow.h

RESOURCES += \
    tseditor.qrc

OTHER_FILES += \
    tseditor.rc

TRANSLATIONS +=


target.path=$${PREFIX}/bin
INSTALLS += target
