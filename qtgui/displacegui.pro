TEMPLATE=app
QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = displacegui

INCLUDEPATH+=../QMapControl/src/
LIBS+=-L. -lqmapcontrol

macx {
    CONFIG -= app_bundle
}


SOURCES += \
    main.cpp \
    mainwindow.cpp

FORMS += \
    mainwindow.ui

HEADERS += \
    mainwindow.h
