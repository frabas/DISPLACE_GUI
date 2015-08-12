QT=
CONFIG=
TARGET=unittest

DESTDIR = ../

DEFINES=
INCLUDEPATH=../include ../commons
LIBS=-L.. -ldisplacecommons -lpthread -lrt -lboost_unit_test_framework

SOURCES= main.cpp \
    dtrees.cpp

HEADERS=

