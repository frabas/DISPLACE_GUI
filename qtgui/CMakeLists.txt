
file(GLOB SOURCES *.cpp
        algo/*.cpp
        backgrounds/*.cpp
        dialogs/*.cpp
        mapobjects/*.cpp
        modelobjects/*.cpp
        mousemode/*.cpp
        objects/*.cpp
        plots/*.cpp
        R/*.cpp
        stats/*.cpp
        utils/*.cpp
        workers/*.cpp
        )

file(GLOB HEADERS *.h
        algo/*.h
        backgrounds/*.h
        dialogs/*.h
        mapobjects/*.h
        modelobjects/*.h
        mousemode/*.h
        objects/*.h
        plots/*.h
        R/*.h
        stats/*.h
        utils/*.h
        workers/*.h
        )

file(GLOB UIS *.ui dialogs/*.ui R/*.ui mapobjects/*.ui)
file(GLOB QRCS *.qrc)


add_executable(displacegui
        MACOSX_BUNDLE
        ${SOURCES} ${HEADERS}
        qtgui.rc
        ${UIS} ${QRCS}
        ../qtcommons/PlotWidget.cpp ../qtcommons/PlotWidget.h ../qtcommons/StatsPlot.h plots/BaseStatsPlotImpl.cpp plots/BaseStatsPlotImpl.h algo/AStarShortestPath.cpp algo/AStarShortestPath.h algo/DijkstraShortestPath.cpp algo/DijkstraShortestPath.h algo/ShortestPathAlgorithm.h)

if (WIN32)
    set_property(TARGET displacegui PROPERTY WIN32_EXECUTABLE yes)
endif (WIN32)

target_link_libraries(displacegui
        commons
        formats
        qtcommons
        QMapControl
        Qt6::Widgets
        Qt6::Concurrent
        Qt6::Sql
        Qt6::Xml
        Qt6::PrintSupport
        Qt6::Network
        CGAL::CGAL
        GDAL::GDAL
        ${GeographicLib_LIBRARIES}
        ${EXTRA_LIBRARIES}
        )

target_include_directories(displacegui
        PRIVATE
        ${GDAL_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/commons
        ${CMAKE_SOURCE_DIR}/formats
        ${CMAKE_SOURCE_DIR}/qtcommons
        ${CMAKE_SOURCE_DIR}/QMapControl/QMapControl/src/
        ${CMAKE_BINARY_DIR}         # For UI headers
        )

add_dependencies(displacegui displace)

# Set target directory
install(TARGETS displacegui
        BUNDLE DESTINATION . COMPONENT gui
        RUNTIME DESTINATION bin COMPONENT gui)
