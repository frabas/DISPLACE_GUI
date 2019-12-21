
if (NOT WIN32)
    message(FATAL "platform_win.cmake must be included only on Win32 interface")
endif()

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS on)

add_definitions(-D_WINSOCKAPI_)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_HAS_ITERATOR_DEBUGGING=1 -D_ITERATOR_DEBUG_LEVEL=2 -DDEBUG)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")     # Visual studio
    message("Apply MSVC fixups")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /permissive-")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")

    # Fix warning C4275
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4275")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP -wd4275")

    # Fix visual studio Bug with boo: error C2116: 'boost::interprocess::winapi::CreateDirectoryA'
    add_definitions(-DBOOST_USE_WINDOWS_H -DNOMINMAX)

    message("MSVC: ${CMAKE_CXX_FLAGS}")
endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

