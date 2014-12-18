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
        ../commons/mkpath.cpp \
        ../commons/memoryinfo.cpp \
        ../commons/options.cpp


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
        ../include/options.h

LIBS=
CPPFLAGS=-O2 -I../include
LDFLAGS=

OBJS=$(SOURCES:.cpp=.o)

displace: ${OBJS}
	${CXX} -o $@ ${LDFLAGS} $^

