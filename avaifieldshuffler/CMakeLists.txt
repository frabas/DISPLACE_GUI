include_directories(
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(avaifieldshuffler
        main.cpp
        )

target_link_libraries(avaifieldshuffler
        commons
        )
