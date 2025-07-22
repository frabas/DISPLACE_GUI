# Sets requirements, check and properties for the compilers

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS}")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif ()

if (WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif ()
