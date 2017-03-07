TEMPLATE = app
QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = scheduler

INCLUDEPATH += ../include/ ../commons ../qtcommons ../formats/

include ("$$top_srcdir/localconfig.pri")

LIBS += -lqtcommons

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
    schedulerscriptgenerator.cpp

HEADERS  += mainwindow.h \
    schedulerjob.h \
    simulationrun.h \
    schedulerjobadapter.h \
    rundialog.h \
    batchcreatedialog.h \
    schedulerscriptgenerator.h

FORMS    += mainwindow.ui \
    rundialog.ui \
    batchcreatedialog.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    scheduler.rc

target.path=$${PREFIX}/bin
INSTALLS += target
