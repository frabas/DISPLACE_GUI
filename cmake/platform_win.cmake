
if (NOT WIN32)
    message(FATAL "platform_win.cmake must be included only on Win32 interface")
endif()

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS on)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")     # Visual studio
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /permissive-")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")

    # Fix warning C4275
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4275")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP -wd4275")

    # Fix visual studio Bug with boo: error C2116: 'boost::interprocess::winapi::CreateDirectoryA'
    add_definitions(-DNOMINMAX)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-D_WIN32_WINNT=0x0602)

    message("MSVC: ${CMAKE_CXX_FLAGS}")
endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

