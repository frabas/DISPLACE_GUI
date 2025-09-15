project (formats_test)

set(SOURCES
        main.cpp
        keyvaluereader.cpp
        linenumberreader.cpp
    )

add_executable(test_formats
        ${SOURCES}
    )

target_link_libraries(test_formats
        formats
        Boost::boost
        )

target_include_directories(test_formats
        PRIVATE
        ../
)


install (TARGETS test_formats RUNTIME DESTINATION bin COMPONENT tests)