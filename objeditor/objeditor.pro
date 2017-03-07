TEMPLATE=app
QT += core gui printsupport concurrent
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = objeditor

INCLUDEPATH+=../include/  ../formats ../qtcommons/

include ("$$top_srcdir/localconfig.pri")

win32 {
    DESTDIR = ..
}
!win32 {
    DESTDIR = ../bin
}

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
win32 {
    QMAPCONTROL_LIB=$${QMAPCONTROL_LIB}1
    CGAL_LIBS= -lCGAL-vc140-mt-4.9 -lgmp -lboost_system-vc140-mt-1_62
}

LIBS += -L.. -l$$QMAPCONTROL_LIB $$CGAL_LIBS

# Add GDAL library path and library (windows).
win32:LIBS += -L$$QMC_GDAL_LIB -lgdal_i

# Add GDAL library path and library (unix).
unix:LIBS += -lgdal

macx:LIBS += -L/Library/Frameworks/GDAL.framework/unix/lib/


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
    RC_FILE = objeditor.rc
}

LIBS += -L.. -lformats -lqtcommons

SOURCES += \
    vesseleditor.cpp \
    vesseleditormainwindow.cpp \
    defaults.cpp \
    settings.cpp \
    csvspecspage.cpp \
    runscriptspage.cpp \
    mapshapefilelistadapter.cpp \
    mapcontrolgraphicsmodel.cpp \
    singlepointmapmodel.cpp \
    linestringmapmodel.cpp

FORMS += \
    vesseleditormainwindow.ui \
    scriptselectionform.ui \
    csvspecspage.ui \
    runscriptspage.ui

HEADERS += \
    vesseleditormainwindow.h \
    defaults.h \
    settings.h \
    csvspecspage.h \
    runscriptspage.h \
    mapshapefilelistadapter.h \
    mapcontrolgraphicsmodel.h \
    singlepointmapmodel.h \
    linestringmapmodel.h

RESOURCES += \
    vesseleditor.qrc

OTHER_FILES += \
    objeditor.rc

TRANSLATIONS +=


target.path=$${PREFIX}/bin
INSTALLS += target

DISTFILES += \
    ../scripts/GenerateVesselsConfigFiles.R \
    ../scripts/RunVesselsConfigFiles.R
