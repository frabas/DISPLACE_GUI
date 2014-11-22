DEFINES += QMC_GDAL

win32 {
    EXTRA_DIR = $$top_srcdir/install/extra/usr/local/
    QMC_GDAL_INC = $$EXTRA_DIR/include
    QMC_GDAL_LIB = $$EXTRA_DIR/lib
}

unix {
    isEmpty(PREFIX) {
        PREFIX=/usr/local/
    }
}

INSTALLS = target
