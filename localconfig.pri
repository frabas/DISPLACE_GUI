DEFINES += QMC_GDAL

win32 {
    #MINGW=C:\Qt\qt-5.3.1-x64-mingw482r4-seh-opengl\mingw64
    MINGW=C:\mingw-w64\x86_64-4.9.2-posix-seh-rt_v3-rev0\mingw64

    QMC_GDAL_INC = $$MINGW/include
    QMC_GDAL_LIB = $$MINGW/lib

    INCLUDEPATH += $$MINGW/include/ $$MINGW/include/boost-1_57
    LIBS += -L$$MINGW/lib
}

unix {
    isEmpty(PREFIX) {
        PREFIX=/usr/local/
    }
}

DESTDIR=$$top_builddir

INSTALLS = target
