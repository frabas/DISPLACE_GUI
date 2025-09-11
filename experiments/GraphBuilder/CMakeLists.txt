cmake_minimum_required(VERSION 3.6)
project(GraphBuilderUsingGdal)

set(CMAKE_CXX_STANDARD 11)

if (APPLE)
    add_definitions(-DBUILD_MACOS -DHOME="$ENV{HOME}")
elseif(UNIX)
    add_definitions(-DBUILD_LINUX -DHOME="$ENV{HOME}")
endif()

set(SOURCE_FILES main.cpp)

add_executable(GraphBuilderUsingGdal ${SOURCE_FILES})

target_link_libraries(GraphBuilderUsingGdal
        GDAL::GDAL)
