project (simulator)

SET(SIMSOURCES
        main.cpp Simulation.cpp Simulation.h Environment.h Actor.h env/Playground.cpp env/Playground.h env/Clock.cpp env/Clock.h env/Calendar.cpp env/Calendar.h)

include_directories(.)

add_executable(displace
        ${SIMSOURCES}
        )

target_link_libraries(displace
        commons
        )

# Set standard as c++11
if (${CMAKE_MAJOR_VERSION} GREATER 2)
    target_compile_features(displace PRIVATE cxx_range_for)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# Set target directory
install (TARGETS displace DESTINATION bin)

