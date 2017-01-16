cmake_minimum_required(VERSION 3.6)
project(GraphBuilderUsingGdal)

set(CMAKE_CXX_STANDARD 11)

include_directories(
        /Library/Frameworks/GDAL.framework/Versions/1.11/unix/include
)

link_directories(
        /usr/include/gdal
        /usr/local/lib
        /Library/Frameworks/GDAL.framework/Versions/1.11/unix/lib
)

set(SOURCE_FILES main.cpp)
add_executable(GraphBuilderUsingGdal ${SOURCE_FILES})

target_link_libraries(GraphBuilderUsingGdal
        gdal)