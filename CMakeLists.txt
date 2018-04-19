cmake_minimum_required(VERSION 3.3)
project(displace)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

## Common packages to all modules
find_package(Boost 1.55 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_subdirectory(formats)
add_subdirectory(commons)
add_subdirectory(QMapControl)

