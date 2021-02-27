add_executable(Citymap WIN32
        src/main.cpp
        src/dialogs.cpp src/dialogs.h
        src/citymap.cpp src/citymap.h
        )

target_include_directories(Citymap
        PRIVATE
        ${CMAKE_SOURCE_DIR}/QMapControl/src
        ${GDAL_INCLUDE_DIR}
        ${PROJ4_INCLUDE_DIR}
        )

target_link_libraries(Citymap QMapControl ${PROJ4_LIBRARIES})

if (INSTALL_EXAMPLES)
    # Set target directory
    install(TARGETS Citymap
            LIBRARY DESTINATION bin
            ARCHIVE DESTINATION bin
            COMPONENT examples)
endif ()
