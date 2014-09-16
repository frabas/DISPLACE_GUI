TEMPLATE=app
QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = displacegui

INCLUDEPATH+=../QMapControl/src/
LIBS+=-L.. -lqmapcontrol

DESTDIR = ../

macx {
    CONFIG -= app_bundle
}


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    wrapperwidget.cpp \
    objecttreemodel.cpp

FORMS += \
    mainwindow.ui

HEADERS += \
    mainwindow.h \
    wrapperwidget.h \
    objecttreemodel.h
