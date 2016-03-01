TEMPLATE=app
QT += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dtreeeditor

INCLUDEPATH+=../include/  ../commons/ ../qtcommons/

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
    RC_FILE = dteditor.rc
}

LIBS+=-L.. -ldisplacecommons



SOURCES += \
    dteditor.cpp \
    dteditorwindow.cpp \
    graphnodeextra.cpp \
    dtgraphicsscene.cpp \
    graphnodeitem.cpp \
    dtcsvreader.cpp \
    dtcsvwriter.cpp \
    commands/command.cpp \
    commands/settreetypecommand.cpp \
    commands/setnodevaluecommand.cpp \
    nodemappingsdialog.cpp

FORMS += \
    dteditorwindow.ui \
    nodemappingsdialog.ui

HEADERS += \
    dteditorwindow.h \
    graphnodeextra.h \
    dtgraphicsscene.h \
    graphnodeitem.h \
    dtcsvreader.h \
    dtcsvwriter.h \
    commands/command.h \
    commands/settreetypecommand.h \
    commands/setnodevaluecommand.h \
    nodemappingsdialog.h

RESOURCES += \
    dteditor.qrc

OTHER_FILES += \
    dteditor.rc

TRANSLATIONS +=


target.path=$${PREFIX}/bin

