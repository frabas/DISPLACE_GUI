# Sets requirements, check and properties for the compilers

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS}")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif ()

# Hack for silly Windows SDK/Qt6 bug generating error C2872> byte ambiguous symbol
add_definitions(-D_HAS_STD_BYTE=0)