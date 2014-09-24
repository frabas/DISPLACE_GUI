TEMPLATE=app
QT += core gui network
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = displacegui

INCLUDEPATH+=../include/ ../QMapControl/QMapControl/src/

DESTDIR = ../
QMAPCONTROL_LIB=qmapcontrol

CONFIG(debug,release|debug) {
    QMAPCONTROL_LIB=qmapcontrold
}
CONFIG(release,release|debug) {
    QMAPCONTROL_LIB=qmapcontrol
}

macx {
    CONFIG -= app_bundle
    ICON = icons/displace.icns
}

win32 {
    RC_FILE = qtgui.rc
    QMAPCONTROL_LIB=$${QMAPCONTROL_LIB}1
}

LIBS+=-L.. -ldisplacecommons -L../QMapControl/QMapControl/src/QMapControl/lib -l$$QMAPCONTROL_LIB

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    wrapperwidget.cpp \
    objecttreemodel.cpp \
    displacemodel.cpp \
    simulator.cpp \
    objects/objecttreeentity.cpp \
    objects/layerentity.cpp \
    objects/harbourentity.cpp \
    scenario.cpp \
    scenariodialog.cpp \
    config.cpp \
    objects/nodeentity.cpp \
    mapobjects/harbourmapobject.cpp \
    mapobjects/mapobject.cpp \
    mapobjectscontroller.cpp \
    mapobjects/nodemapobject.cpp \
    objects/vesselentity.cpp \
    mapobjects/vesselmapobject.cpp

FORMS += \
    mainwindow.ui \
    scenariodialog.ui

HEADERS += \
    mainwindow.h \
    wrapperwidget.h \
    objecttreemodel.h \
    displacemodel.h \
    simulator.h \
    objects/objecttreeentity.h \
    objects/layerentity.h \
    objects/harbourentity.h \
    scenario.h \
    exceptions.h \
    scenariodialog.h \
    config.h \
    objects/nodeentity.h \
    mapobjects/harbourmapobject.h \
    mapobjects/mapobject.h \
    mapobjectscontroller.h \
    mapobjects/nodemapobject.h \
    objects/vesselentity.h \
    mapobjects/vesselmapobject.h

RESOURCES += \
    qtgui.qrc

OTHER_FILES += \
    qtgui.rc
