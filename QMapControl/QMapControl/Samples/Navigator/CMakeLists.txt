add_executable(Navigator
        Navigator.cpp Navigator.h Navigator.qrc
        )


target_include_directories(Navigator
        PRIVATE
        ${CMAKE_SOURCE_DIR}/QMapControl/src
        ${GDAL_INCLUDE_DIR}
        ${PROJ4_INCLUDE_DIR}
        )

target_link_libraries(Navigator QMapControl ${PROJ4_LIBRARIES})

if (INSTALL_EXAMPLES)
    # Set target directory
    install(TARGETS Navigator
            LIBRARY DESTINATION bin
            ARCHIVE DESTINATION bin
            COMPONENT examples)
endif ()
