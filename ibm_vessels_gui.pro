#-------------------------------------------------
#
# Project created by QtCreator 2012-11-16T09:42:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ibm_vessels_gui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Vessel.cpp \
    myutils.cpp \
    readdata.cpp \
    Population.cpp \
    Harbour.cpp \
    Metier.cpp \
    exportdata.cpp \
    Node.cpp \
    myRutils.cpp \
    VesselGUI.cpp \
    mapview.cpp \
    MygraphicsView.cpp \
    Tac.cpp \
    Ship.cpp \
    Benthos.cpp

HEADERS  += mainwindow.h \
    myutils.h \
    readdata.h \
    Metier.h \
    exportdata.h \
    Harbour.h \
    Node.h \
    myRutils.h \
    Population.h \
    Vessel.h \
    VesselGUI.h \
    mapview.h \
    MyGraphicsView.h \
    Tac.h \
    Ship.h \
    Benthos.h

FORMS    += mainwindow.ui


RESOURCES += \
    VesselGUI.qrc

# install
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS ibm_vessels_gui.pro images
INSTALLS +=  sources

