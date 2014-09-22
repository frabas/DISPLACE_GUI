CONFIG -= qt
TARGET=displace
TEMPLATE=app

DESTDIR = ../
INCLUDEPATH=../include

LIBS+= -L../ -ldisplacecommons

macx {
    CONFIG -= app_bundle
}

SOURCES=Benthos.cpp \
        Harbour.cpp \
        Metier.cpp \
        Ship.cpp \
        Vessel.cpp \
        main.cpp
