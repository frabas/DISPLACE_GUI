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

add_executable(scheduler
        ${SOURCES} ${HEADERS}
        scheduler.rc
        ${UIS} ${QRCS}
        )

if (WIN32)
    set_property(TARGET scheduler PROPERTY WIN32_EXECUTABLE yes)
endif (WIN32)


target_link_libraries(scheduler
        commons
        formats
        qtcommons
        Qt6::Widgets
        )

# Set target directory
install(TARGETS scheduler RUNTIME DESTINATION bin COMPONENT tools)
