project (simulator)

SET(SIMSOURCES
        main.cpp
        Simulation.cpp Simulation.h
        Environment.cpp Environment.h
        Actor.h
        env/Playground.cpp env/Playground.h
        env/Clock.cpp env/Clock.h
        env/Calendar.cpp env/Calendar.h
        env/MapGraph.cpp env/MapGraph.h
        io/Loader.cpp io/Loader.h
        utils/make_unique.h
        io/legacy/LegacyLoader.cpp io/legacy/LegacyLoader.h
        env/Node.cpp env/Node.h
        env/Edge.cpp env/Edge.h
        env/ActorsContainer.h
        actors/Vessel.cpp actors/Vessel.h
        env/legacy/LegacyCalendar.h)

add_definitions(
        -DBOOST_FILESYSTEM_VERSION=3
    )

find_package(Boost REQUIRED
        COMPONENTS regex filesystem system
    )

include_directories(.
        ../
        ${Boost_INCLUDE_DIRS}
    )

add_executable(displace
        ${SIMSOURCES}
    )

target_link_libraries(displace
        formats
        ${Boost_LIBRARY_DIRS} ${Boost_LIBRARIES}
        )

# Set standard as c++11
if (${CMAKE_MAJOR_VERSION} GREATER 2)
    target_compile_features(displace PRIVATE cxx_range_for)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# Set target directory
install (TARGETS displace DESTINATION bin)

