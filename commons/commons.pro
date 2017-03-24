CONFIG -= qt

TEMPLATE= lib
TARGET=displacecommons
DESTDIR=../
CONFIG += c++11

include ("$$top_srcdir/localconfig.pri")
macx:DESTDIR=$$LIBDESTDIR

INCLUDEPATH +=../include/ ../formats .
LIBS += -L.. -lformats

win32 {
    # No crash handler support for Windows
    DEFINES += NO_CRASHHANDLER

    # For GetProcessMemoryInfo()
    LIBS += -lpsapi
}

unix:!macx {
    LIBS += -lrt
}

macx {
    # No crash handler support for MacOS
    DEFINES += NO_CRASHHANDLER
}

SOURCES= \
    readdata.cpp \
    myutils.cpp \
    Population.cpp \
    Fishfarm.cpp \
    Windmill.cpp \
    Node.cpp \
    Tac.cpp \
    Benthos.cpp \
    Harbour.cpp \
    Ship.cpp \
    Vessel.cpp \
    Metier.cpp \
    Firm.cpp \
    myRutils.cpp \
    mkpath.cpp \
    memoryinfo.cpp \
    options.cpp \
    statics.cpp \
    simulation.cpp \
    dtree/decisiontree.cpp \
    dtree/variables.cpp \
    dtree/dtnode.cpp \
    dtree/decisiontreemanager.cpp \
    dtree/externalstatemanager.cpp \
    tseries/timeseries.cpp \
    tseries/timeseriesmanager.cpp \
    utils/CrashHandler.cpp


HEADERS= \
    ../include/readdata.h \
    ../include/myutils.h \
    ../include/Population.h \
    ../include/biomodule.h \
    ../include/Fishfarm.h \
    ../include/Windmill.h \
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
    ../include/memoryinfo.h \
    ../include/helpers.h \
    ../include/options.h \
    ../include/mutexlocker.h \
    ../include/guiproto_struct.h \
    ../include/version.h \
    ../include/utils/make_unique.h \
    ../include/vesselcalendar.h \
    ../include/idtypes.h \
    simulation.h \
    dtree/decisiontree.h \
    dtree/variables.h \
    dtree/dtnode.h \
    dtree/dtnodeextra.h \
    dtree/decisiontreemanager.h \
    dtree/stateevaluator.h \
    dtree/externalstatemanager.h \
    dtree/commonstateevaluators.h \
    ../include/comstructs.h \
    tseries/timeseries.h \
    tseries/timeseriesmanager.h \
    dtree/evaluators/timeseriesevaluator.h \
    utils/CrashHandler.h \
    dtree/vesselsevaluators.h


## Do not add this if you don't support IPC queues

!no_ipc {
    SOURCES += \
        ipcqueue.cpp

    HEADERS += \
        ../include/ipcqueue.h \
        ../include/ipcmsgtypes.h \
}

OTHER_FILES += \
    Makefile.hpc

target.path = $${PREFIX}/lib
headers.path = $${PREFIX}/include/
headers.files = $${HEADERS}

INSTALLS += target
