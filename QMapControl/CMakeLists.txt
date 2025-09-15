cmake_minimum_required(VERSION 3.11)
cmake_policy(SET CMP0048 NEW)

project(QMapControl LANGUAGES CXX VERSION 1.1.101)

find_package(GDAL REQUIRED 2.20)
find_package(PROJ4)
message("GDAL library: ${GDAL_LIBRARY}")
message("GDAL include: ${GDAL_INCLUDE_DIR}")
message("PROJ4 library: ${PROJ4_LIBRARIES}")
message("PROJ4 include: ${PROJ4_INCLUDE_DIR}")

option(INSTALL_EXAMPLES "Install everything including examples" OFF)

include(CPack)
include(GNUInstallDirs)
cpack_add_component(development
        DESCRIPTION "Development files"
        REQUIRED
        )
cpack_add_component(examples
        DESCRIPTION "Examples, demo and tests"
        DISABLED
        )


find_package(Qt6 6.8 COMPONENTS Widgets Core Concurrent Sql Xml PrintSupport Network REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB HEADERS QMapControl/src/QMapControl/*.h)
file(GLOB SOURCES QMapControl/src/QMapControl/*.cpp)

set(LIBRARIES
        ${GDAL_LIBRARY}
        )

add_library(QMapControl SHARED
        ${SOURCES} ${HEADERS}
        ${UIS} ${QRCS}
        QMapControl/src/QMapControl/LayerRaster.cpp QMapControl/src/QMapControl/LayerRaster.h QMapControl/src/QMapControl/AdapterRaster.cpp QMapControl/src/QMapControl/AdapterRaster.h QMapControl/src/QMapControl/utils/spimpl.h QMapControl/src/QMapControl/RasterUtils.cpp QMapControl/src/QMapControl/RasterUtils.h QMapControl/src/QMapControl/PersistentCache.cpp QMapControl/src/QMapControl/PersistentCache.h)

target_compile_definitions(QMapControl
        PRIVATE
        -DQMAPCONTROL_LIBRARY)


target_link_libraries(QMapControl
        ${LIBRARIES}
        Qt6::Widgets
        Qt6::Network
        )

target_include_directories(QMapControl
        PRIVATE
        ${GDAL_INCLUDE_DIRS}
        .
        ${Qt5Widgets_INCLUDE_DIRS}
        .
        ${Qt5Widgets_INCLUDE_DIRS}
        )

#add_subdirectory(QMapControl/Samples)

# Set target directory
install(TARGETS QMapControl
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        COMPONENT development)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/QMapControl/src/QMapControl"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        COMPONENT development
        FILES_MATCHING
        PATTERN "*.h")