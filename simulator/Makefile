SOURCES= main.cpp \
        outputmessage.cpp \
        outputqueuemanager.cpp \
        thread_vessels.cpp \
        messages/exportvmslikeoutputmessage.cpp \
        messages/movevesseloutputmessage.cpp \
        messages/genericconsolestringoutputmessage.cpp \
        messages/vessellogbookoutputmessage.cpp \
        ../commons/ipcqueue.cpp \
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
        ../commons/options.cpp \
        ../commons/statics.cpp

HEADERS= \
        outputmessage.h \
        outputqueuemanager.h \
        thread_vessels.h \
        values.h \
        messages/exportvmslikeoutputmessage.h \
        messages/movevesseloutputmessage.h \
        messages/genericconsolestringoutputmessage.h \
        messages/vessellogbookoutputmessage.h \
        ../include/ipcqueue.h \
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

LIBS=-L../commons -lcommons -lpthread -lrt
CPPFLAGS=-O2 -fpermissive -I../include -I../commons -I.
LDFLAGS=

OBJS=$(SOURCES:.cpp=.o)

displace: ${OBJS}
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS}
	
	
clean:
	-@rm ${OBJS} displace

