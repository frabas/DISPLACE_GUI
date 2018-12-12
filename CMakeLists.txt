cmake_minimum_required(VERSION 3.3)
project(displace VERSION 1.0 LANGUAGES C CXX)

include (cmake/options.cmake)
include (cmake/compiler.cmake)
include (cmake/withconan.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include (cmake/qt.cmake)

if (WIN32)
    include (cmake/platform_win.cmake)
endif()

if (APPLE)
    include(cmake/platform_apple.cmake)
endif (APPLE)

include(cmake/installer.cmake)

add_definitions(-DBOOST_ALL_NO_LIB)

include(cmake/dependencies.cmake)
find_package(msqlitecpp REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_subdirectory(formats)
add_subdirectory(commons)
add_subdirectory(QMapControl)
add_subdirectory(qtcommons)
add_subdirectory(qtgui)
add_subdirectory(simulator)

## Tools

add_subdirectory(avaifieldshuffler)
add_subdirectory(avaifieldupdater)
add_subdirectory(vmsmerger)

## Gui TOOLS

add_subdirectory(dteditor)
add_subdirectory(objeditor)
add_subdirectory(tseditor)
add_subdirectory(scheduler)

## Test

add_subdirectory(tests)
add_subdirectory(commons/commons_tests)

