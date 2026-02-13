cmake_minimum_required(VERSION 3.20)
project(displace VERSION 1.0 LANGUAGES C CXX)

set(VCPKG_MANIFEST_INSTALL OFF)

include (cmake/options.cmake)
include (cmake/compiler.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include (cmake/qt.cmake)
 
if (UNIX)
    include(cmake/platform-linux.cmake)
endif ()
if (WIN32)
    include (cmake/platform_win.cmake)
endif()

if (APPLE)
    include(cmake/platform_apple.cmake)
endif (APPLE)

include(cmake/installer.cmake)

add_definitions(-DBOOST_ALL_NO_LIB)

include(cmake/dependencies.cmake)

add_subdirectory(formats)
add_subdirectory(commons)
if (NOT WITHOUT_GUI)
    add_subdirectory(QMapControl)
    add_subdirectory(qtcommons)
    add_subdirectory(qtgui)
endif ()
add_subdirectory(simulator)

## Tools

add_subdirectory(avaifieldshuffler)
add_subdirectory(avaifieldupdater)
add_subdirectory(vmsmerger)

## Gui TOOLS

if (NOT WITHOUT_GUI)
    add_subdirectory(dteditor)
    add_subdirectory(objeditor)
    add_subdirectory(tseditor)
    add_subdirectory(scheduler)
endif ()

## Test

if (NOT WITHOUT_GUI)
    add_subdirectory(tests)
endif ()

if (WITH_TESTS)
    add_subdirectory(commons/commons_tests)
    # Disabled, still not up to date
    #add_subdirectory(simulator/tests)
endif ()
