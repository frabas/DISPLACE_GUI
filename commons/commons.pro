TEMPLATE= lib
TARGET=displacecommons
DESTDIR=../

include ("$$top_srcdir/localconfig.pri")

INCLUDEPATH=../include/

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
    memoryinfo.cpp

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
    ../include/memoryinfo.h

target.path = $${PREFIX}/lib
headers.path = $${PREFIX}/include/
headers.files = $${HEADERS}
