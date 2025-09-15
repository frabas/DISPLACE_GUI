add_executable(avaifieldshuffler
        main.cpp
        )

target_link_libraries(avaifieldshuffler
        commons
        )

target_include_directories(avaifieldshuffler
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Set target directory
install(TARGETS avaifieldshuffler RUNTIME DESTINATION bin COMPONENT tools)
