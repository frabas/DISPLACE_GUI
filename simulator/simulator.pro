CONFIG -= qt
TARGET=displace
TEMPLATE=app

DESTDIR = ../
INCLUDEPATH=../include

include ("$$top_srcdir/localconfig.pri")

QMAKE_CXXFLAGS +=
LIBS+= -L.. -ldisplacecommons -lpthread -lm -lrt

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
DEFINES += PROFILE

# Force callgrind profiling
DEFINES += INSTRUMENTATION

#QMAKE_CXXFLAGS += -g

SOURCES= main.cpp \
    thread_vessels.cpp

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
    ../include/profiler.h \
    thread_vessels.h \
    values.h

OTHER_FILES += \
    Makefile

target.path=$${PREFIX}/bin
