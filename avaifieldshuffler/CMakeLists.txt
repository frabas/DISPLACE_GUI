include_directories(
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(avaifieldshuffler
        main.cpp
        )

target_link_libraries(avaifieldshuffler
        commons
        )


# Set target directory
install(TARGETS avaifieldshuffler RUNTIME DESTINATION bin COMPONENT tools)
