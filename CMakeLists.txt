cmake_minimum_required(VERSION 3.3)
project(displace)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if (WIN32)
    if (NOT BOOST_ROOT)
        set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/install/msvc)
    endif (NOT BOOST_ROOT)
    add_definitions(-DBOOST_ALL_NO_LIB -D_WINSOCKAPI_)
endif (WIN32)

## Common packages to all modules
find_package(Boost 1.55 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_subdirectory(formats)
add_subdirectory(commons)
add_subdirectory(QMapControl)
add_subdirectory(qtcommons)
add_subdirectory(qtgui)
