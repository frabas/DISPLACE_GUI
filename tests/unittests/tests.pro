include ("$$top_srcdir/localconfig.pri")

QT=core
TARGET=unittest
CONFIG += c++14

DESTDIR = ../

DEFINES=
INCLUDEPATH+=../include ../commons ../sparsepp ../formats

macx {
    DESTDIR=$$EXEDESTDIR
    CONFIG -= app_bundle

    INCLUDEPATH += /usr/local/include
    LIBS += -L$$LIBDESTDIR
}

LIBS+=-L.. -lformats -ldisplacecommons

unix:!macx{
    LIBS += -lpthread -lrt
}

unix {
    LIBS+=-lboost_unit_test_framework
}
win32 {
    LIBS += -lboost_unit_test_framework$${BOOST_SUFFIX}
}

SOURCES= main.cpp \
    dtrees.cpp \
    timeseries.cpp \
    rutils.cpp \
    datacollector.cpp

HEADERS=

OTHER_FILES += \
    ../data/tests/dtree-metiers/GoFishing_tree_some_metiers_only.dt.csv \
    ../data/tests/dtree-metiers-mapped/GoFishingMapped.dt.csv \
    ../data/tests/timeseries/fishprice-all_area-4.dat

