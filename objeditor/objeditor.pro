TEMPLATE=app
QT += core gui printsupport concurrent
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = objeditor

INCLUDEPATH+=../include/  ../formats ../qtcommons/

include ("$$top_srcdir/localconfig.pri")

win32 {
    DESTDIR = ..
}
!win32 {
    DESTDIR = ../bin
}

macx {
    DESTDIR=$$EXEDESTDIR
    CONFIG -= app_bundle
}

CONFIG(debug,release|debug) {
    DEFINES += DEBUG
}
CONFIG(release,release|debug) {
}

macx {
    CONFIG -= app_bundle
    #ICON = icons/vesselteditor.icns
}

win32 {
    RC_FILE = objeditor.rc
}

LIBS += -L.. -lformats -lqtcommons

SOURCES += \
    vesseleditor.cpp \
    vesseleditormainwindow.cpp \
    scriptselectionform.cpp \
    defaults.cpp \
    settings.cpp \
    csvspecspage.cpp \
    runscriptspage.cpp

FORMS += \
    vesseleditormainwindow.ui \
    scriptselectionform.ui \
    csvspecspage.ui \
    runscriptspage.ui

HEADERS += \
    vesseleditormainwindow.h \
    scriptselectionform.h \
    defaults.h \
    settings.h \
    csvspecspage.h \
    runscriptspage.h

RESOURCES += \
    vesseleditor.qrc

OTHER_FILES += \
    objeditor.rc

TRANSLATIONS +=


target.path=$${PREFIX}/bin
INSTALLS += target

DISTFILES += \
    ../scripts/GenerateVesselsConfigFiles.R \
    ../scripts/RunVesselsConfigFiles.R
