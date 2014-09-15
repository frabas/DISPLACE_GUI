QT += widgets network

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
