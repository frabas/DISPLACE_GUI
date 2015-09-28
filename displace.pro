TEMPLATE=subdirs
CONFIG+=ordered
SUBDIRS+=commons qtcommons QMapControl qtgui simulator dteditor tseditor vesseleditor

!win32 {
    CONFIG += unit-test
    SUBDIRS += tests
}

!unit-test {
    message("Unit-test disabled")
}

OTHER_FILES += \
    Readme.txt \
    Building.win Building.unix \
    localconfig.pri \
    COPYRIGHT.txt \
    LICENSE.txt \
    setup.iss \
    scripts/* \
    data/*

