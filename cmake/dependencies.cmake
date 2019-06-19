# Check dependencies

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

## Common packages to all modules
find_package(Boost 1.55 REQUIRED COMPONENTS date_time system thread program_options unit_test_framework)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

find_package(GeographicLib REQUIRED)
message("GeographicLib version: ${GeographicLib_VERSION}")
if (NOT WITHOUT_GUI)
    find_package(Qt5 5.6 COMPONENTS Widgets Core Concurrent Sql Xml PrintSupport Network REQUIRED)
    include_directories(${GeographicLib_INCLUDE_DIR})
endif ()
if (WITHOUT_GUI)
    include_directories(${GeographicLib_INCLUDE_DIRS})
endif ()
link_directories(${GeographicLib_LIBRARY_DIRS})

find_package(GDAL REQUIRED 1.11)
message("GDAL library: ${GDAL_LIBRARY}")

if (NOT APPLE)
    include_directories(${GDAL_INCLUDE_DIR})
    link_directories(${GDAL_LIBRARY_DIRS})
endif()

if (NOT WITHOUT_GUI)
    find_package(CGAL REQUIRED)
    message(" CGAL version: ${CGAL_VERSION} ")
    include_directories(${CGAL_INCLUDE_DIR})
    link_directories(${CGAL_LIBRARY_DIRS})
endif ()

find_package(Sparsepp REQUIRED)

find_package(msqlitecpp REQUIRED)
include_directories(${sparsepp_INCLUDE_DIR})
link_directories(${sparsepp_LIBRARY_DIRS})
