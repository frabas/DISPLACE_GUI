project (commons)

SET(SOURCES
        ipcqueue.cpp
        dtree/decisiontree.cpp
        dtree/variables.cpp
        dtree/dtnode.cpp
        dtree/decisiontreemanager.cpp
        dtree/externalstatemanager.cpp
        tseries/timeseries.cpp
        tseries/timeseriesmanager.cpp
        include/ipcqueue.h include/ipcmsgtypes.h
    )

find_package(Boost REQUIRED)

message(STATUS "Boost found: ${Boost_INCLUDE_DIRS}")

include_directories(
        include
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
