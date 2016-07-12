TEMPLATE=app
QT += core gui printsupport concurrent
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vsleditor

INCLUDEPATH+=../include/  ../formats

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
    #ICON = icons/vesselteditor.icns
}

win32 {
    RC_FILE = vesseleditor.rc
}

LIBS += -L.. -lformats

SOURCES += \
    vesseleditor.cpp \
    vesseleditormainwindow.cpp \
    vesselsspec.cpp \
    vesselsspecmodel.cpp

FORMS += \
    vesseleditormainwindow.ui

HEADERS += \
    vesseleditormainwindow.h \
    vesselsspec.h \
    vesselsspecmodel.h

RESOURCES += \
    vesseleditor.qrc

OTHER_FILES += \
    vesseleditor.rc

TRANSLATIONS +=


target.path=$${PREFIX}/bin
INSTALLS += target
