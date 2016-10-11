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


LIBS += -lqtcommons

include ("$$top_srcdir/QMapControl/QMapControl/QMapControl.pri")

INCLUDEPATH+=../QMapControl/QMapControl/src/QMapControl/

QMAPCONTROL_LIB=qmapcontrol

CONFIG(debug,release|debug) {
    DEFINES += DEBUG
    QMAPCONTROL_LIB=qmapcontrold
}
CONFIG(release,release|debug) {
    QMAPCONTROL_LIB=qmapcontrol
}

LIBS += -L.. -l$$QMAPCONTROL_LIB $$CGAL_LIBS

# Add GDAL library path and library (windows).
win32:LIBS += -L$$QMC_GDAL_LIB -lgdal

# Add GDAL library path and library (unix).
unix:LIBS += -lgdal

macx:LIBS += -L/Library/Frameworks/GDAL.framework/unix/lib/

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
