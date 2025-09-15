add_executable(vmsmerger
        main.cpp
        )

target_link_libraries(vmsmerger
        commons
        )

target_include_directories(vmsmerger
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Set target directory
install(TARGETS vmsmerger RUNTIME DESTINATION bin COMPONENT tools)
