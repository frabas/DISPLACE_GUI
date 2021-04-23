add_executable(MapViewer
        src/main.cpp
        src/mapviewer.cpp src/mapviewer.h
        ../resources/Resources.qrc
        )

target_include_directories(MapViewer
        PRIVATE
        ${CMAKE_SOURCE_DIR}/QMapControl/src
        )

target_link_libraries(MapViewer QMapControl)
