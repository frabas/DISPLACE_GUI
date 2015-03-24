TEMPLATE=app
QT += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dtreeeditor

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
    #RC_FILE = dteditor.rc
}

LIBS+=-L.. -ldisplacecommons



SOURCES += \
    dteditor.cpp \
    dteditorwindow.cpp \
    graphnodeextra.cpp \
    dtgraphicsscene.cpp \
    graphnodeitem.cpp \
    dtcsvreader.cpp \
    dtcsvwriter.cpp

FORMS += \
    dteditorwindow.ui

HEADERS += \
    dteditorwindow.h \
    graphnodeextra.h \
    dtgraphicsscene.h \
    graphnodeitem.h \
    dtcsvreader.h \
    dtcsvwriter.h

RESOURCES += \
    dteditor.qrc

OTHER_FILES += \
    dteditor.rc

TRANSLATIONS +=


target.path=$${PREFIX}/bin

