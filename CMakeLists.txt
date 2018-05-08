cmake_minimum_required(VERSION 3.3)
project(displace)

option(DISABLE_IPC "Disable the IPC support (gui/simulator channel)" OFF)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message("Debug build")
endif (CMAKE_BUILD_TYPE MATCHES Debug)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
    if (NOT BOOST_ROOT)
        set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/install/msvc)
    endif (NOT BOOST_ROOT)

    add_definitions(-DBOOST_ALL_NO_LIB -D_WINSOCKAPI_)

    if (CMAKE_BUILD_TYPE MATCHES Debug)
        add_definitions(-D_HAS_ITERATOR_DEBUGGING=0 -D_SECURE_SCL=1 -DDEBUG)
    endif (CMAKE_BUILD_TYPE MATCHES Debug)
endif (WIN32)

if (APPLE)
    link_directories(/Library/Frameworks/GDAL.framework/unix/lib/)
endif (APPLE)

## Common packages to all modules
find_package(Boost 1.55 REQUIRED)

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

