CONFIG -= qt
TARGET=displace
TEMPLATE=app

DESTDIR = ../
INCLUDEPATH=../include

LIBS+=

macx {
    CONFIG -= app_bundle
}

win32 {
    #PATH+=C:\mingw\x64-4.8.1-posix-seh-rev5\mingw64\bin
    QMAKE_CXX = x86_64-w64-mingw32-g++
}

SOURCES= main.cpp \
    ../commons/readdata.cpp \
    ../commons/myutils.cpp \
    ../commons/Population.cpp \
    ../commons/Node.cpp \
    ../commons/Tac.cpp \
    ../commons/Benthos.cpp \
    ../commons/Harbour.cpp \
    ../commons/Ship.cpp \
    ../commons/Vessel.cpp \
    ../commons/Metier.cpp \
    ../commons/myRutils.cpp \
    ../commons/mkpath.cpp


HEADERS= \
    ../include/readdata.h \
    ../include/myutils.h \
    ../include/Population.h \
    ../include/Node.h \
    ../include/Tac.h \
    ../include/Benthos.h \
    ../include/Harbour.h \
    ../include/Ship.h \
    ../include/Vessel.h \
    ../include/Metier.h \
    ../include/myRutils.h \
    ../include/mkpath.h \
    ../include/m_constants.h

OTHER_FILES += \
    Makefile
