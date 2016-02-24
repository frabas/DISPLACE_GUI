DEFINES += QMC_GDAL

win32 {
    #MINGW=C:\Qt\qt-5.3.1-x64-mingw482r4-seh-opengl\mingw64
    #MINGW=C:\mingw-w64\x86_64-4.9.2-posix-seh-rt_v3-rev0\mingw64
    #SDK=$$MINGW

    SDK=$$top_srcdir/install/extra

    QMC_GDAL_INC = $$SDK/include
    QMC_GDAL_LIB = $$SDK/lib

    INCLUDEPATH += $$SDK/include/ $$SDK/include/boost-1_57
    LIBS += -L$$SDK/lib
    
    CONFIG += boost_test_included
}

unix {
    isEmpty(PREFIX) {
        PREFIX=/usr/local/
    }
    #CONFIG += boost_test_included

}

macx {
    INCLUDEPATH += /usr/local/include
    INCLUDEPATH += /Library/Frameworks/GDAL.framework/unix/include

    LIBS += -L/usr/local/lib
    LIBS += -L/Library/Frameworks/GDAL.framework/unix/lib/ -lgdal
    DEFINES +=

    QMAKE_CXXFLAGS += -std=c++11

}

DESTDIR=$$top_builddir

INSTALLS = target
