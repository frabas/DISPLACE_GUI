
add_definitions(-DQMAPCONTROL_LIBRARY)

find_package(Qt5Widgets 5.6 REQUIRED)
find_package(Qt5Network 5.6 REQUIRED)

include_directories(
        .
        ${Qt5Widgets_INCLUDE_DIRS}
)

file(GLOB HEADERS QMapControl/src/QMapControl/*.h)
file(GLOB SOURCES QMapControl/src/QMapControl/*.cpp)

if (APPLE)
    link_directories(/Library/Frameworks/GDAL.framework/unix/lib/)
endif (APPLE)

set(LIBRARIES
        gdal
        )

add_library(QMapControl SHARED ${SOURCES} ${HEADERS})

target_link_libraries(QMapControl
        ${LIBRARIES}
        Qt5::Widgets
        Qt5::Network
        )
