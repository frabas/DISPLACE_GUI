add_executable(runLoaderTest
        ../data/NodesLoader.cpp ../data/NodesLoader.h
        ../data/EdgesLoader.cpp ../data/EdgesLoader.h
        main.cpp
        LoaderTestSuite.cpp LoaderTestSuite.h ../../commons/DatabaseInputImpl/VesselsLoader.cpp ../../commons/DatabaseInputImpl/VesselsLoader.h ../data/ClosuresLoader.cpp ../data/ClosuresLoader.h)

target_include_directories(runLoaderTest
        PRIVATE
        ${CMAKE_SOURCE_DIR}/simulator/data
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/formats
        ${CMAKE_SOURCE_DIR}/commons
        )

if (NOT APPLE AND NOT WIN32)
    list(APPEND OS_LIBRARIES pthread rt)
endif (NOT APPLE AND NOT WIN32)


target_link_libraries(runLoaderTest
        commons
        Boost::program_options
        Boost::log
        ${OS_LIBRARIES}
        )