
include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/commons
        ${CMAKE_SOURCE_DIR}/formats
        ${CMAKE_SOURCE_DIR}/qtcommons
        ${CMAKE_SOURCE_DIR}/QMapControl/QMapControl/src/
        ${CMAKE_BINARY_DIR}         # For UI headers
        ${Sparsepp_INCLUDE_DIRS}
        ${Qt5Widgest_INCLUDE_DIRS}
        ${Qt5PrintSupport_INCLUDE_DIRS}
)

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

set(EXTRA_LIBRARIES
        )
if (WIN32)
    link_directories(${CMAKE_SOURCE_DIR}/install/msvc/lib)
    if (CMAKE_BUILD_TYPE MATCHES Release)
        list(APPEND EXTRA_LIBRARIES
                CGAL::CGAL
                ${GeographicLib_LIBRARIES}
                )
    else (CMAKE_BUILD_TYPE MATCHES Release)
        list(APPEND EXTRA_LIBRARIES
                CGAL::CGAL
                ${GeographicLib_LIBRARIES}
                )
    endif (CMAKE_BUILD_TYPE MATCHES Release)
else (WIN32)
    list(APPEND EXTRA_LIBRARIES
            CGAL::CGAL
            ${GeographicLib_LIBRARIES}
            sqlite3
            )
endif (WIN32)

if (NOT APPLE AND NOT WIN32)
    list(APPEND EXTRA_LIBRARIES CGAL::CGAL gmp rt)
endif (NOT APPLE AND NOT WIN32)

if (APPLE)
    list(APPEND EXTRA_LIBRARIES CGAL::CGAL gmp)
endif ()

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
        Qt5::Widgets
        Qt5::Concurrent
        Qt5::Sql
        Qt5::Xml
        Qt5::PrintSupport
        Qt5::Network

        ${EXTRA_LIBRARIES}
        )

target_include_directories(displacegui
        PRIVATE
        ${GDAL_INCLUDE_DIRS}
        )

# Set target directory
install(TARGETS displacegui
        BUNDLE DESTINATION . COMPONENT gui
        RUNTIME DESTINATION bin COMPONENT gui)
