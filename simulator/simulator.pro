CONFIG -= qt
TARGET=displace
TEMPLATE=app

DESTDIR = ../
INCLUDEPATH=../include

LIBS+= -L../ -ldisplacecommons

macx {
    CONFIG -= app_bundle
}

SOURCES= main.cpp
