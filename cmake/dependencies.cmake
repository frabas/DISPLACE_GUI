# Check dependencies

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

## Common packages to all modules
find_package(Boost 1.55 REQUIRED COMPONENTS date_time system thread)
include_directories( ${Boost_INCLUDE_DIR} )
link_directories( ${Boost_LIBRARY_DIRS} )

find_package(GeographicLib REQUIRED)
message ("GeographicLib version: ${GeographicLib_VERSION}")
if (NOT WITHOUT_GUI) include_directories( ${GeographicLib_INCLUDE_DIR} )
if (WITHOUT_GUI) include_directories( ${GeographicLib_INCLUDE_DIRS} )
link_directories( ${GeographicLib_LIBRARY_DIRS} )

find_package(GDAL REQUIRED 1.11)
message ("GDAL library: ${GDAL_LIBRARY}")
include_directories( ${GDAL_INCLUDE_DIR} )
link_directories( ${GDAL_LIBRARY_DIRS} )

if (NOT WITHOUT_GUI)
find_package(CGAL REQUIRED)
message ("CGAL version: ${CGAL_VERSION}")
include_directories( ${CGAL_INCLUDE_DIR} )
link_directories( ${CGAL_LIBRARY_DIRS} )
endif ()

find_package(Sparsepp REQUIRED)

find_package(msqlitecpp REQUIRED)
include_directories( ${sparsepp_INCLUDE_DIR} )
link_directories( ${sparsepp_LIBRARY_DIRS} )
