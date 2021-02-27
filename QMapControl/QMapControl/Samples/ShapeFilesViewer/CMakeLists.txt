add_executable(ShapeFilesViewer
        ShapeFilesViewer.cpp ShapeFilesViewer.h
        )


target_include_directories(ShapeFilesViewer
        PRIVATE
        ${CMAKE_SOURCE_DIR}/QMapControl/src
        ${GDAL_INCLUDE_DIR}
        ${PROJ4_INCLUDE_DIR}
        )

target_link_libraries(ShapeFilesViewer QMapControl  ${PROJ4_LIBRARIES})

if (INSTALL_EXAMPLES)
    # Set target directory
    install(TARGETS ShapeFilesViewer
            LIBRARY DESTINATION bin
            ARCHIVE DESTINATION bin
            COMPONENT examples)
endif ()