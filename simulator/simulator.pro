CONFIG += console

TARGET=displace
TEMPLATE=app

DESTDIR = ../

INCLUDEPATH=../include ../commons ../formats ../sparsepp

include ("$$top_srcdir/localconfig.pri")
macx {
    DESTDIR=$$EXEDESTDIR
    CONFIG -= app_bundle
}

QMAKE_CXXFLAGS += -fpermissive
CONFIG += c++14

LIBS+= -L.. -ldisplacecommons -lformats -lmsqlitecpp

!win32: -lsqlite3

macx {
    #LIBS += -L/Library/Frameworks/GDAL.framework/unix/lib/
    CONFIG -= app_bundle
}

unix:LIBS += -lboost_program_options -lboost_filesystem -lpthread

win32 {
    # For GetProcessMemoryInfo()
    DEFINES += BOOST_ALL_DYN_LINK
    LIBS += -lpsapi -lboost_program_options$${BOOST_SUFFIX}
}

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

CONFIG(profile) {
    DEFINES += INSTRUMENTATION
    message("Instrumentation enabled")
}

#QMAKE_CXXFLAGS += -g

SOURCES= main.cpp \
    thread_vessels.cpp \
    outputexporter.cpp \
    getRSS.cpp \
    biomodule2.cpp \
    fisheriesmanagmt.cpp \
    runtime_tests/NodeTester.cpp

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
    ../include/Firm.h \
    ../include/myRutils.h \
    ../include/mkpath.h \
    ../include/m_constants.h \
    ../include/options.h \
    ../include/profiler.h \
    ../include/version.h \
    runtime_tests/NodeTester.h \
    thread_vessels.h \
    values.h \
    messages/noipc.h \
    outputexporter.h \
    getrss.h \
    biomodule2.h

unix:!macx {
    SOURCES += memstats.cpp
    HEADERS += memstats.h
}


!no_ipc {
    SOURCES += \
        ipc.cpp \
        outputqueuemanager.cpp \
        outputmessage.cpp \
        messages/movevesseloutputmessage.cpp \
        messages/vessellogbookoutputmessage.cpp \
        messages/genericconsolestringoutputmessage.cpp \
        messages/moveshipoutputmessage.cpp

    HEADERS += \
        ipc.h \
        outputqueuemanager.h \
        outputmessage.h \
        messages/movevesseloutputmessage.h \
        messages/vessellogbookoutputmessage.h \
        messages/genericconsolestringoutputmessage.h \
        messages/moveshipoutputmessage.h
}

### mSQLiteCpp dependency

INCLUDEPATH += ../mSqliteCpp/include
#SOURCES += ../mSqliteCpp/src/*
HEADERS += ../mSqliteCpp/include/*
LIBS +=

### End mSqliteCpp Depedency


OTHER_FILES += \
    Makefile.hpc

target.path=$${PREFIX}/bin
INSTALLS += target
