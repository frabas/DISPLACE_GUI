# Add support for c++11.
CONFIG += c++11

# Hide debug output in release mode.
CONFIG(release, debug|release) : DEFINES += QT_NO_DEBUG_OUTPUT

isEmpty(top_srcdir) {
    # This file is located in a subdirectory of the top source dir.
    top_srcdir=$$PWD/..
    top_builddir=$$shadowed($$PWD/..)
}

# Required defines.
DEFINES +=                          \
    # Windows: To force math constants to be defined in <cmath>
    _USE_MATH_DEFINES               \
    # Windows: Extend MSVC 11 to allow more than 5 arguments for its 'faux' variadic templates.
    # @todo remove once MSVC supports standards-based variadic templates.
    _VARIADIC_MAX=6                 \

# Build MOC directory.
MOC_DIR = tmp

# Build object directory.
OBJECTS_DIR = obj

# Build resources directory.
RCC_DIR = resources

# Build UI directory.
UI_DIR = ui

# Add Qt modules.
QT +=                               \
    network                         \
    widgets                         \

# Variables for clients

QMAPCONTROL_LIB=qmapcontrol
CONFIG(debug,release|debug) {
    QMAPCONTROL_LIB=qmapcontrold
}

win32 {
    QMAPCONTROL_LIB=$${QMAPCONTROL_LIB}1
    CGAL_LIBS= -lCGAL-vc140-mt-4.9 -lgmp -lboost_system-vc140-mt-1_63
}

unix {
    CGAL_LIBS= -lCGAL -lgmp
}
