find_package(Boost REQUIRED
        COMPONENTS program_options
        )

add_definitions(-DBOOST_PROGRAM_OPTIONS_DYN_LINK)

include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/commons
        ${CMAKE_SOURCE_DIR}/formats
        ${CMAKE_SOURCE_DIR}/sparsepp
        ${CMAKE_SOURCE_DIR}/mSqliteCpp/include
)

set(SOURCES
        main.cpp
        thread_vessels.cpp
        outputexporter.cpp
        getRSS.cpp
        biomodule2.cpp
        )

if (WIN32)
    link_directories(${CMAKE_SOURCE_DIR}/install/msvc/lib)
else(WIN32)
    list (APPEND LIBRARIES sqlite3)
endif (WIN32)

if (NOT APPLE AND NOT WIN32)
    list(APPEND SOURCES
            memstats.cpp)
    list(APPEND LIBRARIES pthread rt)
endif (NOT APPLE AND NOT WIN32)

if (NOT DISABLE_IPC)
    list(APPEND SOURCES
            ipc.cpp
            outputqueuemanager.cpp
            outputmessage.cpp
            messages/movevesseloutputmessage.cpp
            messages/vessellogbookoutputmessage.cpp
            messages/genericconsolestringoutputmessage.cpp
            messages/moveshipoutputmessage.cpp
            )
endif (NOT DISABLE_IPC)

add_executable(displace
        ${SOURCES} ${HEADERS}
        )


target_link_libraries(displace
        commons
        formats
        ${LIBRARIES}
        Boost::program_options
        )