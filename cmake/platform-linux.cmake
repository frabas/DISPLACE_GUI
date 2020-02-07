if (NOT UNIX)
    message(FATAL "platform_win.cmake must be included only on UNIX interface")
endif ()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/debian)
