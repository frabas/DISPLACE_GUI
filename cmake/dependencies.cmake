# Check dependencies

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

## Common packages to all modules
find_package(Boost 1.55 REQUIRED COMPONENTS date_time filesystem system thread program_options log unit_test_framework)

add_definitions(-DBOOST_ALL_NO_LIB_LIB)

find_package(GeographicLib REQUIRED)
message("GeographicLib version: ${GeographicLib_VERSION}")
if (NOT WITHOUT_GUI)
    find_package(Qt6 6.8 COMPONENTS Widgets Core Concurrent Sql Xml PrintSupport Network REQUIRED)
endif ()

if (WITHOUT_GUI)
    include_directories(${GeographicLib_INCLUDE_DIRS})
endif ()
# link_directories(${GeographicLib_LIBRARY_DIRS})

find_package(GDAL REQUIRED 1.11)
message("GDAL library: ${GDAL_LIBRARY}")
message("GDAL include: ${GDAL_INCLUDE_DIR}")

if (NOT WITHOUT_GUI)
    find_package(CGAL REQUIRED)
    message(" CGAL version: ${CGAL_VERSION} ")
endif ()

find_package(Sparsepp REQUIRED)
find_package(msqlitecpp REQUIRED)
