SOURCES= main.cpp \
        outputmessage.cpp \
        outputqueuemanager.cpp \
        thread_vessels.cpp \
        messages/exportvmslikeoutputmessage.cpp \
        messages/movevesseloutputmessage.cpp \
        messages/moveshipoutputmessage.cpp \
        messages/genericconsolestringoutputmessage.cpp \
        messages/vessellogbookoutputmessage.cpp

HEADERS= \
        outputmessage.h \
        outputqueuemanager.h \
        thread_vessels.h \
        values.h \
        messages/exportvmslikeoutputmessage.h \
        messages/movevesseloutputmessage.h \
        messages/moveshipoutputmessage.h \
        messages/genericconsolestringoutputmessage.h \
        messages/vessellogbookoutputmessage.h

LIBS=-L../commons -lcommons -lpthread -lrt
CPPFLAGS=${CPPDEBUG} -O2 -fpermissive -I../include -I../commons -I.
LDFLAGS=

OBJS=$(SOURCES:.cpp=.o)

all: displace

displace: ${OBJS}
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS}

debug:
	${MAKE} all CPPDEBUG=-g

clean:
	-@rm ${OBJS} displace

