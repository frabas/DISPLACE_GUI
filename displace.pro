TEMPLATE=subdirs
CONFIG+=ordered
SUBDIRS+=commons qtcommons QMapControl qtgui simulator dteditor tseditor vesseleditor

unix,!macx{
    CONFIG += unit-test
    SUBDIRS += tests
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

