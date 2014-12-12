CONFIG -= qt
TARGET=displace
TEMPLATE=app

DESTDIR = ../
INCLUDEPATH=../include

include ("$$top_srcdir/localconfig.pri")

LIBS+= -L.. -ldisplacecommons

macx {
    CONFIG -= app_bundle
}

win32 {
    #PATH+=C:\mingw\x64-4.8.1-posix-seh-rev5\mingw64\bin
#    QMAKE_CXX = x86_64-w64-mingw32-g++
#    QMAKE_LINK = x86_64-w64-mingw32-g++
#    QMAKE_LFLAGS = -static
}

CONFIG(debug,debug|release) {
    DEFINES += DEBUG PROFILE
}

# Force profiling in release
DEFINES += PROFILE INSTRUMENTATION

SOURCES= main.cpp

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
    ../include/m_constants.h \
    ../include/options.h \
    ../include/profiler.h

OTHER_FILES += \
    Makefile

target.path=$${PREFIX}/bin
