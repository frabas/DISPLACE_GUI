TEMPLATE=app
QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = displacegui

INCLUDEPATH+=../include/ ../QMapControl/src/

DESTDIR = ../
QMAPCONTROL_LIB=qmapcontrol

macx {
    CONFIG -= app_bundle
    ICON = icons/displace.icns
}

win32 {
    RC_FILE = qtgui.rc
    QMAPCONTROL_LIB=qmapcontrol0
}

LIBS+=-L.. -ldisplacecommons -l$$QMAPCONTROL_LIB

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    wrapperwidget.cpp \
    objecttreemodel.cpp \
    displacemodel.cpp \
    simulator.cpp \
    objects/objecttreeentity.cpp \
    objects/layerentity.cpp \
    objects/harbourentity.cpp

FORMS += \
    mainwindow.ui

HEADERS += \
    mainwindow.h \
    wrapperwidget.h \
    objecttreemodel.h \
    displacemodel.h \
    simulator.h \
    objects/objecttreeentity.h \
    objects/layerentity.h \
    objects/harbourentity.h

RESOURCES += \
    qtgui.qrc

OTHER_FILES += \
    qtgui.rc
