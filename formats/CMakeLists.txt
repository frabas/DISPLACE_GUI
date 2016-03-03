project (formats)

find_package(Boost REQUIRED)

set(SOURCES
        legacy/ConfigFileReader.cpp legacy/ConfigFileReader.h
        utils/KeyValueReader.cpp utils/KeyValueReader.h
    )

include_directories(
        ${Boost_INCLUDE_DIRS}
    )

set(CMAKE_CXX_FLAGS "-fpermissive" )

add_library(formats
        ${SOURCES}
        )


#target_link_libraries(
#    )

# Set standard as c++11
if (${CMAKE_MAJOR_VERSION} GREATER 2)
    target_compile_features(formats PRIVATE cxx_range_for)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# Set target directory
install (TARGETS formats DESTINATION lib)
