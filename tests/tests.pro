include ("$$top_srcdir/localconfig.pri")

QT=
TARGET=unittest
CONFIG += c++11

DESTDIR = ../

DEFINES=
INCLUDEPATH=../include ../commons

macx {
    DESTDIR=$$EXEDESTDIR
    CONFIG -= app_bundle

    INCLUDEPATH += /usr/local/include
    LIBS += -L$$LIBDESTDIR
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
    dtrees.cpp \
    timeseries.cpp \
    rutils.cpp

HEADERS=

OTHER_FILES += \
    ../data/tests/dtree-metiers/GoFishing_tree_some_metiers_only.dt.csv \
    ../data/tests/dtree-metiers-mapped/GoFishingMapped.dt.csv \
    ../data/tests/timeseries/fishprice-all_area-4.dat

