include ("$$top_srcdir/localconfig.pri")

QT=core
TARGET=commons_tests
CONFIG += c++11

DESTDIR = ../bin/test

DEFINES=
INCLUDEPATH+=../../include .. ../../sparsepp

macx {
    DESTDIR=$$EXEDESTDIR
    CONFIG -= app_bundle

    INCLUDEPATH += /usr/local/include
    LIBS += -L../$$LIBDESTDIR
}

LIBS+=-L../.. -lformats -ldisplacecommons

unix:!macx{
    LIBS += -lpthread -lrt
}

unix {
    LIBS+=-lboost_unit_test_framework
}
win32 {
    LIBS += -lboost_unit_test_framework-vc140-mt-1_63
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
    inputfiles_simusspe.cpp \
    inputfiles_windmillsspe.cpp \
    test_pathshop.cpp \
    sqlitestorage.cpp

HEADERS=


### mSQLiteCpp dependency

INCLUDEPATH += $$top_srcdir/mSqliteCpp/include
#SOURCES += $$top_srcdir/mSqliteCpp/src/*
HEADERS += $$top_srcdir/mSqliteCpp/include/*
LIBS +=

### End mSqliteCpp Depedency

OTHER_FILES +=
