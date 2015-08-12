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
    ipcqueue.cpp \
    statics.cpp \
    simulation.cpp \
    dtree/decisiontree.cpp \
    dtree/variables.cpp \
    dtree/dtnode.cpp \
    dtree/decisiontreemanager.cpp \
    dtree/externalstatemanager.cpp \
    tseries/timeseries.cpp \
    tseries/timeseriesmanager.cpp

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
    ../include/ipcmsgtypes.h \
    ../include/version.h \
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
    dtree/evaluators/timeseriesevaluator.h

LIBS=-lpthread -lrt
CPPFLAGS=${CPPDEBUG} -O2 -fpermissive -I../include -I../commons -I. -fpic
LDFLAGS=

OBJS=$(SOURCES:.cpp=.o)

all: libcommons.so

debug:
	${MAKE} all CPPDEBUG=-g

libcommons.so: ${OBJS}
	${CXX} -shared ${LDFLAGS} -o $@ $^ ${LIBS}
	
	
	
clean:
	@-rm ${OBJS} libcommons.so

