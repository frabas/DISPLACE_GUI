TEMPLATE= lib
TARGET=displacecommons
DESTDIR=../

include ("$$top_srcdir/localconfig.pri")

INCLUDEPATH=../include/

win32 {
    # For GetProcessMemoryInfo()
    LIBS += -lpsapi
}

SOURCES= \
    readdata.cpp \
    myutils.cpp \
    Population.cpp \
    Node.cpp \
    Tac.cpp \
    Benthos.cpp \
    Harbour.cpp \
    Ship.cpp \
    Vessel.cpp \
    Metier.cpp \
    myRutils.cpp \
    mkpath.cpp \
    memoryinfo.cpp \
    options.cpp \
    ipcqueue.cpp

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
    ../include/memoryinfo.h \
    ../include/helpers.h \
    ../include/options.h \
    ../include/mutexlocker.h \
    ../include/guiproto_struct.h \
    ../include/ipcqueue.h \
    ../include/ipcmsgtypes.h

target.path = $${PREFIX}/lib
headers.path = $${PREFIX}/include/
headers.files = $${HEADERS}
