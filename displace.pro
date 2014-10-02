TEMPLATE=subdirs
CONFIG+=ordered
SUBDIRS+=commons QMapControl qtgui

!win {
    SUBDIRS += simulator
}

OTHER_FILES += \
    Readme.txt
