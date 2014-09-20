TEMPLATE=app
QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = displacegui

INCLUDEPATH+=../QMapControl/src/ ../simulator

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

LIBS+=-L.. -l$$QMAPCONTROL_LIB

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    ../simulator/readdata.cpp \
    ../simulator/myutils.cpp \
    ../simulator/Population.cpp \
    ../simulator/Node.cpp \
    ../simulator/Tac.cpp \
    ../simulator/myRutils.cpp \
    wrapperwidget.cpp \
    objecttreemodel.cpp \
    displacemodel.cpp \
    objects/Ships.cpp

FORMS += \
    mainwindow.ui

HEADERS += \
    mainwindow.h \
    ../simulator/readdata.h \
    ../simulator/myutils.h \
    ../simulator/Population.h \
    ../simulator/Node.h \
    ../simulator/Tac.h \
    ../simulator/myRutils.h \
    wrapperwidget.h \
    objecttreemodel.h \
    displacemodel.h \
    objects/Ships.h

RESOURCES += \
    qtgui.qrc

OTHER_FILES += \
    qtgui.rc
