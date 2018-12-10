find_package(Qt5Core 5.6 REQUIRED)
find_package(Qt5Widgets 5.6 REQUIRED)
find_package(Qt5PrintSupport 5.6 REQUIRED)

add_definitions(-DQTCOMMONS_LIBRARY -DQCUSTOMPLOT_COMPILE_LIBRARY)

include_directories(
        ${CMAKE_SOURCE_DIR}/QMapControl/QMapControl/src/QMapControl/
        ${Qt5Widgest_INCLUDE_DIRS}
        ${Qt5PrintSupport_INCLUDE_DIRS}
)

file(GLOB SOURCES *.cpp
        csv/*.cpp graphics/*.cpp R/*.cpp
        )
file(GLOB HEADERS *.h
        csv/*.h graphics/*.h R/*.h
        )

file(GLOB QRCS *.qrc)

add_library(qtcommons SHARED
        ${SOURCES} ${HEADERS}
        ${UIS} ${QRCS}
        )


target_link_libraries(qtcommons
        QMapControl
        Qt5::Widgets
        Qt5::PrintSupport
        )

# Set target directory
install(TARGETS qtcommons
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        COMPONENT gui)
