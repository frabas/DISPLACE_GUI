include ("$$top_srcdir/localconfig.pri")

QT=
TARGET=commons_tests
CONFIG += c++11

DESTDIR = ../bin/test

DEFINES=
INCLUDEPATH=../../include ..

macx {
    DESTDIR=$$EXEDESTDIR
    CONFIG -= app_bundle

    INCLUDEPATH += /usr/local/include
    LIBS += -L../$$LIBDESTDIR
}

LIBS+=-L.. -lformats -ldisplacecommons

!macx{
    LIBS += -lpthread -lrt
}

!boost_test_included {
    message("Boost::Test framework will be dynamically linked")
    LIBS+=-lboost_unit_test_framework
}
boost_test_included {
    message("Boost::Test framework will be compiled in")
    DEFINES += HAVE_BOOST_TEST_COMPILED
}

SOURCES= main.cpp \
    files.cpp \
    inputfiles_graphsspe.cpp \
    inputfiles_popsspe.cpp \
    inputfiles_vesselsspe.cpp \
    inputfiles_shipsspe.cpp \
    inputfiles_metiersspe.cpp \
    inputfiles_harboursspe.cpp \
    inputfiles_benthosspe.cpp \
    inputfiles_fishfarmsspe.cpp \
    inputfiles_simusspe.cpp

HEADERS=

OTHER_FILES +=
