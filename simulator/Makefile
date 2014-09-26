GCCPATH=C:\mingw\x64-4.8.1-posix-seh-rev5\mingw64\bin
CXX=$(GCCPATH)/x86_64-w64-mingw32-g++
LD=ld

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

LIBS=
CPPFLAGS=-O2
LDFLAGS=

OBJS=$(SOURCES:.cpp=.o)

displace: ${OBJS}
        $(LD) -o $@ ${LDFLAGS} $^
