project (simulator)

SET(SIMSOURCES
        main.cpp)

add_executable(displace
        ${SIMSOURCES}
        )

# Set standard as c++11
if (${CMAKE_MAJOR_VERSION} GREATER 2)
    target_compile_features(displace PRIVATE cxx_range_for)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# Set target directory
install (TARGETS displace DESTINATION bin)
