CONFIG -= qt
CONFIG +=

TARGET=displace
TEMPLATE=app

DESTDIR = ../
INCLUDEPATH=../include ../commons

include ("$$top_srcdir/localconfig.pri")

QMAKE_CXXFLAGS += -fpermissive
LIBS+= -L.. -ldisplacecommons

LIBS += -lpthread

unix {
    LIBS += -lrt
}


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
    DEFINES += DEBUG
    !macx {
        DEFINES += PROFILE
    }
}

CONFIG(release,debug|release) {
    # Force profiling in release
    !macx {
        DEFINES += PROFILE
    }
}

# Force callgrind profiling
DEFINES += INSTRUMENTATION

#QMAKE_CXXFLAGS += -g

SOURCES= main.cpp \
    thread_vessels.cpp \
    outputqueuemanager.cpp \
    outputmessage.cpp \
    messages/movevesseloutputmessage.cpp \
    messages/exportvmslikeoutputmessage.cpp \
    messages/vessellogbookoutputmessage.cpp \
    messages/genericconsolestringoutputmessage.cpp \
    messages/moveshipoutputmessage.cpp

HEADERS= \
    ../include/readdata.h \
    ../include/myutils.h \
    ../include/Population.h \
    ../include/Fishfarm.h \
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
    ../include/version.h \
    thread_vessels.h \
    values.h \
    outputqueuemanager.h \
    outputmessage.h \
    messages/movevesseloutputmessage.h \
    messages/exportvmslikeoutputmessage.h \
    messages/vessellogbookoutputmessage.h \
    messages/genericconsolestringoutputmessage.h \
    messages/moveshipoutputmessage.h

OTHER_FILES += \
    Makefile.hpc

target.path=$${PREFIX}/bin
