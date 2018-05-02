include_directories(
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(vmsmerger
        main.cpp
        )

target_link_libraries(vmsmerger
        commons
        )
