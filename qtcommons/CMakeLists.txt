find_package(Qt5Core 5.6 REQUIRED)
find_package(Qt5Widgets 5.6 REQUIRED)
find_package(Qt5PrintSupport 5.6 REQUIRED)

add_definitions(-DQTCOMMONS_LIBRARY -DQCUSTOMPLOT_COMPILE_LIBRARY)

include_directories(
        ${CMAKE_SOURCE_DIR}/QMapControl/QMapControl/src/QMapControl/
        ${Qt5Widgest_INCLUDE_DIRS}
        ${Qt5PrintSupport_INCLUDE_DIRS}
)

file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS *.h)
file(GLOB_RECURSE UIS *.ui)
file(GLOB_RECURSE QRCS *.qrc)

add_library(qtcommons SHARED
        ${SOURCES} ${HEADERS}
        ${UIS} ${QRCS}
        )


target_link_libraries(qtcommons
        Qt5::Widgets
        Qt5::PrintSupport
        )
