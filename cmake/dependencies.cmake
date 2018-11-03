# Check dependencies

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

## Common packages to all modules
find_package(Boost 1.55 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

find_package(GeographicLib REQUIRED)
message ("GeographicLib version: ${GeographicLib_VERSION}")

find_package(GDAL REQUIRED 1.11)
message ("GDAL library: ${GDAL_LIBRARY}")

find_package(CGAL REQUIRED)
message ("CGAL version: ${CGAL_VERSION}")

find_package(Sparsepp REQUIRED)

find_package(MSqliteCpp REQUIRED)
