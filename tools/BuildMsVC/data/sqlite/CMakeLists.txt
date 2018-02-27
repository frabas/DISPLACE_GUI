cmake_minimum_required(VERSION 3.0)
project(sqlite_amalgamation_3210000)

set(CMAKE_CXX_STANDARD 11)

set(SRC
        shell.c
        sqlite3.c
        sqlite3.h
        sqlite3ext.h
        )

add_library(sqlite3 SHARED  ${SRC})
add_library(sqlite3_static STATIC ${SRC})

