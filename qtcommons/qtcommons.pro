QT       += widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

CONFIG += c++14

TARGET = qtcommons
TEMPLATE = lib
DESTDIR=../

DEFINES += QTCOMMONS_LIBRARY QCUSTOMPLOT_COMPILE_LIBRARY

include ("$$top_srcdir/localconfig.pri")
macx:DESTDIR=$$LIBDESTDIR

include (../QMapControl/QMapControl/QMapControl.pri)

INCLUDEPATH+=../QMapControl/QMapControl/src/QMapControl/

win32: LIBS += -L.. -lformats -l$$QMAPCONTROL_LIB  $$CGAL_LIBS
macx: LIBS += -L.. -lformats -l$$QMAPCONTROL_LIB  $$CGAL_LIBS

SOURCES += qcustomplot.cpp \
        R/env.cpp \
        csv/csvimporter.cpp \
        csv/csvexporter.cpp \
        csv/csvtablemodel.cpp \
    R/defaults.cpp \
    R/settings.cpp \
    appsettings.cpp \
    graphics/fishfarmgraphics.cpp \
    graphics/harbourgraphics.cpp \
    graphics/shiplanesgraphics.cpp \
    graphics/windmillgraphics.cpp \
    graphics/firmgraphics.cpp

HEADERS +=\
        qtcommons_global.h \
        qcustomplot.h \
        R/env.h \
        csv/csvimporter.h \
        csv/csvexporter.h \
        csv/csvtablemodel.h \
    qsignalblocker.h \
    R/defaults.h \
    R/settings.h \
    appsettings.h \
    graphics/fishfarmgraphics.h \
    graphics/harbourgraphics.h \
    graphics/shiplanesgraphics.h \
    graphics/windmillgraphics.h \
    graphics/firmgraphics.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    qtcommons.qrc
