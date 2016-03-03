CONFIG -= qt

TARGET=displace
TEMPLATE=app

DESTDIR = ../

INCLUDEPATH=../include ../commons

include ("$$top_srcdir/localconfig.pri")
macx {
    DESTDIR=$$EXEDESTDIR
    CONFIG -= app_bundle
}

QMAKE_CXXFLAGS += -fpermissive
LIBS+= -L.. -ldisplacecommons

macx {
    CONFIG += c++11
    LIBS += -L/Library/Frameworks/GDAL.framework/unix/lib/
    CONFIG -= app_bundle
}

LIBS += -lpthread

unix:!macx {
    LIBS += -lrt
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
# DEFINES += INSTRUMENTATION

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
INSTALLS += target
