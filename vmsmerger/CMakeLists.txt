include_directories(
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(vmsmerger
        main.cpp
        )

target_link_libraries(vmsmerger
        commons
        )

# Set target directory
install(TARGETS vmsmerger RUNTIME DESTINATION bin COMPONENT tools)
