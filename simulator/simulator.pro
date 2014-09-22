CONFIG -= qt
TARGET=displace
TEMPLATE=app

DESTDIR = ../
INCLUDEPATH=../include

LIBS+= -L../ -ldisplacecommons

SOURCES=Benthos.cpp \
        Harbour.cpp \
        Metier.cpp \
        Ship.cpp \
        Vessel.cpp \
        main.cpp
