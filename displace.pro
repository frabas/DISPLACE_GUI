TEMPLATE=subdirs
CONFIG+=ordered unit-test
SUBDIRS+=commons qtcommons QMapControl qtgui simulator dteditor tseditor

unit-test {
    SUBDIRS += tests
}

!unit-test {
    message("Unit-test disabled")
}

OTHER_FILES += \
    Readme.txt \
    COPYRIGHT.txt \
    LICENSE.txt \
    setup.iss \
    scripts/* \
    data/*

