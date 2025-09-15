set(SOURCES
        main.cpp
        thread_vessels.cpp
        outputexporter.cpp
        getRSS.cpp
        biomodule2.cpp
        fisheriesmanagmt.cpp
        runtime_tests/NodeTester.cpp
        runtime_tests/NodeTester.h
        data/NodesLoader.cpp
        data/NodesLoader.h
        runtime_tests/RuntimeTests.cpp runtime_tests/RuntimeTests.h)

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

target_include_directories(displace
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/commons
        ${CMAKE_SOURCE_DIR}/formats
        ${Sparsepp_INCLUDE_DIRS}
        )

target_link_libraries(displace
        commons
        formats
        ${LIBRARIES}
        msqlitecpp::msqlitecpp

        Boost::program_options
        Boost::filesystem
        )


# Set target directory
install(TARGETS displace RUNTIME DESTINATION bin COMPONENT simulator)
