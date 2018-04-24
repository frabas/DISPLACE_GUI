
add_definitions(-DQMAPCONTROL_LIBRARY)

find_package(Qt5Widgets 5.6 REQUIRED)
find_package(Qt5Network 5.6 REQUIRED)

include_directories(
        .
        ${Qt5Widgets_INCLUDE_DIRS}
)

file(GLOB HEADERS QMapControl/src/QMapControl/*.h)
file(GLOB SOURCES QMapControl/src/QMapControl/*.cpp)

set(GDAL_LIBRARY gdal)

if (APPLE)
    link_directories(/Library/Frameworks/GDAL.framework/unix/lib/)
endif (APPLE)

if (WIN32)
    link_directories(${CMAKE_SOURCE_DIR}/install/msvc/lib)
    set(GDAL_LIBRARY gdal_i)
endif (WIN32)

set(LIBRARIES
        ${GDAL_LIBRARY}
        )

add_library(QMapControl SHARED
        ${SOURCES} ${HEADERS}
        ${UIS} ${QRCS}
        )

target_link_libraries(QMapControl
        ${LIBRARIES}
        Qt5::Widgets
        Qt5::Network
        )
