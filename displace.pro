TEMPLATE=subdirs
CONFIG+=ordered
SUBDIRS+=formats commons QMapControl qtcommons qtgui simulator dteditor tseditor objeditor \
    scheduler \
    avaifieldshuffler \
    avaifieldupdater

CONFIG += unit-test
SUBDIRS += tests \
    commons/commons_tests

unix,!macx {
    SUBDIRS += profile \
}

win32 {
    CONFIG(debug,release|debug){
        DEFINES += _HAS_ITERATOR_DEBUGGING=0
        DEFINES += -D_SECURE_SCL=1
    }
}

commons.depends = formats
qtcommons.depends = QMapControl commons
qtgui.depends = commons qtcommons QMapControl
simulator.depends = commons
scheduler.depends = qtcommons
dteditor.depends = commons qtcommons
tseditor.depends = commons qtcommons
objeditor.depends = commons qtcommons QMapControl
avaifieldshuffler.depends=commons

!unit-test {
    message("Unit-test disabled")
}

OTHER_FILES += \
    Readme.txt \
    Makefile.hpc \
    Building.win Building.unix Building.MacOSX \
    localconfig.pri \
    COPYRIGHT.txt \
    LICENSE.txt \
    setup.iss \
    scripts/* \
    data/*

