find_package(Qt5Core 5.6 REQUIRED)
find_package(Qt5Widgets 5.6 REQUIRED)

include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/commons
        ${CMAKE_SOURCE_DIR}/formats
        ${CMAKE_SOURCE_DIR}/qtcommons
)

file(GLOB SOURCES
        *.cpp
        commands/*.cpp
        )

file(GLOB HEADERS
        *.h
        commands/*.h
        )

file(GLOB UIS *.ui)
file(GLOB QRCS *.qrc)

add_executable(dteditor
        ${SOURCES} ${HEADERS}
        ${UIS} ${QRCS}
        )


target_link_libraries(dteditor
        commons
        formats
        qtcommons
        Qt5::Widgets
        )
