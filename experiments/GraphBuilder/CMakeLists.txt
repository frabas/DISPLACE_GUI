cmake_minimum_required(VERSION 3.6)
project(GraphBuilderUsingGdal)

set(CMAKE_CXX_STANDARD 11)

include_directories( /Library/Frameworks/GDAL.framework/unix/include )
link_directories( /Library/Frameworks/GDAL.framework/unix/lib )

set(SOURCE_FILES main.cpp)
add_executable(GraphBuilderUsingGdal ${SOURCE_FILES})

target_link_libraries(GraphBuilderUsingGdal
        gdal)