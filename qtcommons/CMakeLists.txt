add_definitions(-DQTCOMMONS_LIBRARY -DQCUSTOMPLOT_COMPILE_LIBRARY)

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

target_include_directories(qtcommons
        PRIVATE
        ${CMAKE_SOURCE_DIR}/QMapControl/QMapControl/src/QMapControl/
        )

target_link_libraries(qtcommons
        QMapControl
        Qt6::Widgets
        Qt6::PrintSupport
        )

# Set target directory
install(TARGETS qtcommons
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        COMPONENT gui)
