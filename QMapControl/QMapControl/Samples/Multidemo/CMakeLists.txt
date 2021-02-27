add_executable(Multidemo
        src/main.cpp
        src/gps_modul.cpp src/gps_modul.h
        src/multidemo.cpp src/multidemo.h
        )


target_include_directories(Multidemo
        PRIVATE
        ${CMAKE_SOURCE_DIR}/QMapControl/src
        ${GDAL_INCLUDE_DIR}
        ${PROJ4_INCLUDE_DIR}
        )

target_link_libraries(Multidemo QMapControl ${PROJ4_LIBRARIES})

if (INSTALL_EXAMPLES)
    # Set target directory
    install(TARGETS Multidemo
            LIBRARY DESTINATION bin
            ARCHIVE DESTINATION bin
            COMPONENT examples)
endif ()
