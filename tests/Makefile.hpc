
INCLUDEPATHS=-I../commons
LIBS=-L../commons -lcommons -lboost_unit_test_framework

CPPFLAGS=-DBOOST_TEST_DYN_LINK

tests: dtrees.cpp main.cpp
	g++ -o $@ ${INCLUDEPATHS} -DBOOST_TEST_DYN_LINK $^ ${LIBS} 

