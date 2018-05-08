find_package(Qt5Core 5.6 REQUIRED)
find_package(Qt5Widgets 5.6 REQUIRED)
find_package(Qt5Concurrent 5.6 REQUIRED)
find_package(Qt5Sql 5.6 REQUIRED)
find_package(Qt5Xml 5.6 REQUIRED)
find_package(Qt5PrintSupport 5.6 REQUIRED)
find_package(Qt5Network 5.6 REQUIRED)

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
        
file(GLOB UIS *.ui dialogs/*.ui R/*.ui mapobjects/*.ui )
file(GLOB QRCS *.qrc)

set(EXTRA_LIBRARIES
        )
if (WIN32)
    link_directories(${CMAKE_SOURCE_DIR}/install/msvc/lib)
    if (CMAKE_BUILD_TYPE MATCHES Release)
        list(APPEND EXTRA_LIBRARIES
                CGAL-vc140-mt-4.9
                Geographic
                )
    else(CMAKE_BUILD_TYPE MATCHES Release)
        list(APPEND EXTRA_LIBRARIES
                CGAL-vc140-mt-gd-4.9
                Geographic_d
                )
    endif(CMAKE_BUILD_TYPE MATCHES Release)
else(WIN32)
    list(APPEND EXTRA_LIBRARIES
            CGAL
            Geographic
            sqlite3
            )
endif(WIN32)

add_executable(displacegui
        ${SOURCES} ${HEADERS}
        ${UIS} ${QRCS}
        )


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

        ${EXTRA_LIBRARIES} gmp
        )
