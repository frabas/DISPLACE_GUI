TEMPLATE=subdirs
CONFIG+=ordered
SUBDIRS+=formats commons QMapControl qtcommons qtgui simulator dteditor tseditor objeditor \
    scheduler

unix:!macx{
    CONFIG += unit-test
    SUBDIRS += tests profile \
        commons/commons_tests
}

commons.depends = formats
qtcommons.depends = QMapControl commons
qtgui.depends = commons qtcommons QMapControl
simulator.depends = commons
scheduler.depends = qtcommons
dteditor.depends = commons qtcommons
tseditor.depends = commons qtcommons
objeditor.depends = commons qtcommons QMapControl

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

