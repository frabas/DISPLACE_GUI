add_executable(gps WIN32
        src/main.cpp
        src/gps.cpp src/gps.h
        src/gps_neo.cpp src/gps_neo.h
        )

target_include_directories(gps
        PRIVATE
        ${CMAKE_SOURCE_DIR}/QMapControl/src
        ${GDAL_INCLUDE_DIR}
        ${PROJ4_INCLUDE_DIR}
        )

target_link_libraries(gps QMapControl ${PROJ4_LIBRARIES})

if (INSTALL_EXAMPLES)
    # Set target directory
    install(TARGETS gps
            LIBRARY DESTINATION bin
            ARCHIVE DESTINATION bin
            COMPONENT examples)
endif ()
