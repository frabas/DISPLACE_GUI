find_package(Qt5Core 5.6 REQUIRED)
find_package(Qt5Widgets 5.6 REQUIRED)
find_package(Qt5Concurrent 5.6 REQUIRED)

include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/commons
        ${CMAKE_SOURCE_DIR}/formats
        ${CMAKE_SOURCE_DIR}/qtcommons
)

file(GLOB SOURCES
        *.cpp
        )

file(GLOB HEADERS
        *.h
        )

file(GLOB UIS *.ui)
file(GLOB QRCS *.qrc)

add_executable(tseditor
        ${SOURCES} ${HEADERS}
        tseditor.rc
        ${UIS} ${QRCS}
        )

if (WIN32)
    set_property(TARGET tseditor PROPERTY WIN32_EXECUTABLE yes)
endif (WIN32)

if (NOT APPLE AND NOT WIN32)
    list(APPEND LIBRARIES rt)
endif (NOT APPLE AND NOT WIN32)


target_link_libraries(tseditor
        commons
        formats
        qtcommons
        ${LIBRARIES}
        Qt5::Widgets
        Qt5::Concurrent
        )

# Set target directory
install(TARGETS tseditor RUNTIME DESTINATION bin COMPONENT tools)
