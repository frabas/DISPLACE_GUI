project (formats_test)

find_package(Boost REQUIRED
    COMPONENTS unit_test_framework regex
    )

set(SOURCES
        main.cpp
        keyvaluereader.cpp
        linenumberreader.cpp
    )

include_directories(
        ../
        ${Boost_INCLUDE_DIRS}
)

set(CMAKE_CXX_FLAGS "-fpermissive" )

add_executable(test_formats
        ${SOURCES}
    )

target_link_libraries(test_formats
        formats
        ${Boost_LIBRARY_DIRS} ${Boost_LIBRARIES}
        )

# Set standard as c++11
if (${CMAKE_MAJOR_VERSION} GREATER 2)
    target_compile_features(test_formats PRIVATE cxx_range_for)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

intall (TARGETS test_formats RUNTIME DESTINATION bin COMPONENT tests)