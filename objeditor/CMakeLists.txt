include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/commons
        ${CMAKE_SOURCE_DIR}/formats
        ${CMAKE_SOURCE_DIR}/qtcommons
        ${CMAKE_SOURCE_DIR}/QMapControl/QMapControl/src/QMapControl
)

file(GLOB SOURCES
        *.cpp
        )

file(GLOB HEADERS
        *.h
        )

file(GLOB UIS *.ui)
file(GLOB QRCS *.qrc)

add_executable(objeditor
        ${SOURCES} ${HEADERS}
        objeditor.rc
        ${UIS} ${QRCS}
        )

if (WIN32)
    set_property(TARGET objeditor PROPERTY WIN32_EXECUTABLE yes)
endif (WIN32)


target_link_libraries(objeditor
        commons
        formats
        qtcommons
        Qt5::Widgets
        QMapControl
        )

target_include_directories(objeditor
        PRIVATE
        ${GDAL_INCLUDE_DIRS}
        )

# Set target directory
install(TARGETS objeditor RUNTIME DESTINATION bin COMPONENT tools)
