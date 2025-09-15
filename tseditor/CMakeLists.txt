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

target_include_directories(tseditor
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/commons
        ${CMAKE_SOURCE_DIR}/formats
        ${CMAKE_SOURCE_DIR}/qtcommons
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
        Qt6::Widgets
        Qt6::Concurrent
        )

# Set target directory
install(TARGETS tseditor RUNTIME DESTINATION bin COMPONENT tools)
