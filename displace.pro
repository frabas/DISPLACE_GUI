TEMPLATE=subdirs
CONFIG+=ordered
SUBDIRS+=formats commons qtcommons QMapControl qtgui simulator dteditor tseditor objeditor \
    scheduler

unix:!macx{
    CONFIG += unit-test
    SUBDIRS += tests profile
}

!unit-test {
    message("Unit-test disabled")
}

OTHER_FILES += \
    Readme.txt \
    Building.win Building.unix Building.MacOSX \
    localconfig.pri \
    COPYRIGHT.txt \
    LICENSE.txt \
    setup.iss \
    scripts/* \
    data/*

