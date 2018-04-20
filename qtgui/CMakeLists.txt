find_package(Qt5Core 5.6 REQUIRED)
find_package(Qt5Widgets 5.6 REQUIRED)
find_package(Qt5Concurrent 5.6 REQUIRED)
find_package(Qt5Sql 5.6 REQUIRED)
find_package(Qt5Xml 5.6 REQUIRED)
find_package(Qt5PrintSupport 5.6 REQUIRED)
find_package(Qt5Network 5.6 REQUIRED)

add_definitions(-DQTCOMMONS_LIBRARY -DQCUSTOMPLOT_COMPILE_LIBRARY)

include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/commons
        ${CMAKE_SOURCE_DIR}/formats
        ${CMAKE_SOURCE_DIR}/qtcommons
        ${CMAKE_SOURCE_DIR}/QMapControl/QMapControl/src/
        ${CMAKE_SOURCE_DIR}/sparsepp
        ${CMAKE_SOURCE_DIR}/mSqliteCpp/include
        ${CMAKE_BINARY_DIR}         # For UI headers
        ${Qt5Widgest_INCLUDE_DIRS}
        ${Qt5PrintSupport_INCLUDE_DIRS}
)

file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS *.h)
file(GLOB_RECURSE UIS *.ui)
file(GLOB_RECURSE QRCS *.qrc)

if (APPLE)
    link_directories(/Library/Frameworks/GDAL.framework/unix/lib/)
endif (APPLE)

if (WIN32)
    link_directories(${CMAKE_SOURCE_DIR}/install/msvc/lib)
endif(WIN32)

add_executable(qtgui
        ${SOURCES} ${HEADERS}
        ${UIS} ${QRCS}
        )


target_link_libraries(qtgui
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

        Geographic
        CGAL gmp sqlite3
        )
