include_directories(
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(avaifieldupdater
        main.cpp
        )

target_link_libraries(avaifieldupdater
        commons
        )
