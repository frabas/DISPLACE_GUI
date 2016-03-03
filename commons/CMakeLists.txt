project (commons)

SET(SOURCES
        readdata.cpp
        myutils.cpp
        Population.cpp
        Fishfarm.cpp
        Node.cpp
        Tac.cpp
        Benthos.cpp
        Harbour.cpp
        Ship.cpp
        Vessel.cpp
        Metier.cpp
        myRutils.cpp
        mkpath.cpp
        memoryinfo.cpp
        options.cpp
        ipcqueue.cpp
        statics.cpp
        simulation.cpp
        dtree/decisiontree.cpp
        dtree/variables.cpp
        dtree/dtnode.cpp
        dtree/decisiontreemanager.cpp
        dtree/externalstatemanager.cpp
        tseries/timeseries.cpp
        tseries/timeseriesmanager.cpp
    )

find_package(Boost REQUIRED)

message(STATUS "Boost found: ${Boost_INCLUDE_DIRS}")

include_directories(
        ../include
        ../commons
        ${Boost_INCLUDE_DIRS}
    )

set(CMAKE_CXX_FLAGS "-fpermissive" )

add_library(commons
        ${SOURCES}
    )


target_link_libraries(
        commons
        pthread
    )
