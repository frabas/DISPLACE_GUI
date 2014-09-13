SOURCES=Benthos.cpp \
	Harbour.cpp \
	Metier.cpp \
	Node.cpp \
	Population.cpp \
	Ship.cpp \
	Tac.cpp \
	Vessel.cpp \
	main.cpp \
	myRutils.cpp \
	myutils.cpp \
	readdata.cpp

LIBS=
CPPFLAGS=-O2
LDFLAGS=

OBJS=$(SOURCES:.cpp=.o)



displace: ${OBJS}
	g++ -o $@ ${LDFLAGS} $^

