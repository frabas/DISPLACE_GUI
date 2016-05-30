include ("$$top_srcdir/localconfig.pri")

QT=
TARGET=unittest
CONFIG += c++11

DESTDIR = ../

DEFINES=
INCLUDEPATH=../include ../commons
LIBS=-L.. -lformats -ldisplacecommons -lpthread -lrt

!boost_test_included {
    message("Boost::Test framework will be dynamically linked")
    LIBS+=-lboost_unit_test_framework
}
boost_test_included {
    message("Boost::Test framework will be compiled in")
    DEFINES += HAVE_BOOST_TEST_COMPILED
}

SOURCES= main.cpp \
    dtrees.cpp \
    timeseries.cpp \
    rutils.cpp

HEADERS=

