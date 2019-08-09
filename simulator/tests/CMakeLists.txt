add_executable(runLoaderTest
        ../data/NodesLoader.cpp ../data/NodesLoader.h
        ../data/EdgesLoader.cpp ../data/EdgesLoader.h
        main.cpp
        LoaderTestSuite.cpp LoaderTestSuite.h)

target_include_directories(runLoaderTest
        PRIVATE
        ${CMAKE_SOURCE_DIR}/simulator/data
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/formats
        ${CMAKE_SOURCE_DIR}/commons
        )

target_link_libraries(runLoaderTest
        commons
        Boost::program_options
        Boost::log
        pthread
        )