add_executable(avaifieldupdater
        main.cpp
        )

target_include_directories(avaifieldupdater
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        )

target_link_libraries(avaifieldupdater
        commons
        )

# Set target directory
install(TARGETS avaifieldupdater RUNTIME DESTINATION bin COMPONENT tools)
