DEFINES += QMC_GDAL

CONFIG += c++14

macx {
    APPTARGET=DisplaceProject
}

!macx {
    APPTARGET=displace
}

SRCROOT=$$top_srcdir

CONFIG(profile) {
    message("Profiling enabled")

    QMAKE_CXXFLAGS += -frounding-math
    DEFINES += CGAL_DISABLE_ROUNDING_MATH_CHECK

    CONFIG(gprof) {
        message ("Compiling with support for gProf")
        QMAKE_CXXFLAGS += -pg
        #QMAKE_CXXFLAGS += -fPIC
        #QMAKE_LFLAGS += -pg -static
    }
}


win32 {
    #MINGW=C:\Qt\qt-5.3.1-x64-mingw482r4-seh-opengl\mingw64
    #MINGW=C:\mingw-w64\x86_64-4.9.2-posix-seh-rt_v3-rev0\mingw64
    #SDK=$$MINGW

    CONFIG(debug,release|debug){
        BOOST_SUFFIX = -vc140-mt-gd-1_63
    }
    CONFIG(release,release|debug){
        BOOST_SUFFIX = -vc140-mt-1_63
    }


    DEFINES += BOOST_ALL_NO_LIB _WINSOCKAPI_

    SDK=$$top_srcdir/install/msvc
    #SDK=$$top_srcdir/../displace-build/install

    QMC_GDAL_INC = $$SDK/include
    QMC_GDAL_LIB = $$SDK/lib

    INCLUDEPATH += $$SDK/include/ $$SDK/include
    LIBS += -L$$SDK/lib
    
    CONFIG += boost_test_included
}

unix {
    isEmpty(PREFIX) {
        PREFIX=/usr/
    }
    #CONFIG += boost_test_included
    !macx {
        #LIBS += -L/usr/lib/x86_64-linux-gnu
        LIBS += -lrt
    }

}

macx {
    INCLUDEPATH += /usr/local/include
    INCLUDEPATH += /usr/local/Cellar/boost/1.65.1/include
    #Note: make a soft link in /usr/local/include/gdal pointing to
    INCLUDEPATH += /Library/Frameworks/GDAL.framework/Versions/Current/unix/include

    LIBS += -L/usr/local/lib
    LIBS += -L/usr/local/Cellar/boost/1.65.1/lib
    #Note: make a soft link in /usr/local/lib pointing to libgdal.dylib in the following location:
    LIBS += -L/Library/Frameworks/GDAL.framework/Versions/Current/unix/lib
    DEFINES +=

    APPDESTDIR=$$join(top_builddir,,,"/bin")
    TARGETAPPBUNDLE=$$join(APPTARGET,,,".app")
    TARGETSODIR= $$APPDESTDIR $$TARGETAPPBUNDLE Contents Frameworks
    TARGETEXEDIR= $$APPDESTDIR $$TARGETAPPBUNDLE Contents MacOS
    LIBDESTDIR= $$join(TARGETSODIR,"/")
    EXEDESTDIR= $$join(TARGETEXEDIR,"/")

    message("ExePath: $$EXEDESTDIR")
    message("LibPath: $$LIBDESTDIR")


    ## hack
    QMC_LIBDESTDIR=$$LIBDESTDIR

    QMAKE_LFLAGS_SONAME=-Wl,-install_name,@executable_path/../Frameworks/
    LOCALEDIR=$$APPDESTDIR/$$TARGETAPPBUNDLE/Contents/Resources/Locales

    LIBS += -L$$LIBDESTDIR
}

MOC_DIR = moc
UI_DIR = ui
OBJECTS_DIR = obj
RCC_DIR = obj

DESTDIR=$$top_builddir

HEADERS += \
    $$PWD/include/dbtypes.h \
    $$PWD/include/modeltypes.h \
    $$PWD/include/plottypes.h
