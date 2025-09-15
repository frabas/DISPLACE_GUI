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
        dteditor.rc
        ${UIS} ${QRCS}
        )

target_include_directories(dteditor
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/commons
        ${CMAKE_SOURCE_DIR}/formats
        ${CMAKE_SOURCE_DIR}/qtcommons
        )

if (WIN32)
    set_property(TARGET dteditor PROPERTY WIN32_EXECUTABLE yes)
endif (WIN32)

if (NOT APPLE AND NOT WIN32)
    list(APPEND LIBRARIES rt)
endif (NOT APPLE AND NOT WIN32)

target_link_libraries(dteditor
        commons
        formats
        qtcommons
        ${LIBRARIE}
        Qt6::Widgets
        )

# Set target directory
install(TARGETS dteditor RUNTIME DESTINATION bin COMPONENT tools)
