find_package(Boost REQUIRED
        COMPONENTS unit_test_framework
        )


include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/commons
        ${CMAKE_SOURCE_DIR}/formats
        ${CMAKE_SOURCE_DIR}/sparsepp
)

file(GLOB SOURCES
        *.cpp
        )

file(GLOB HEADERS
        *.h
        )

file(GLOB UIS *.ui)
file(GLOB QRCS *.qrc)

add_executable(commons_tests
        ${SOURCES} ${HEADERS}
        ${UIS} ${QRCS}
        )


target_link_libraries(commons_tests
        commons
        formats
        Boost::unit_test_framework
        )

install(TARGETS commons_tests RUNTIME DESTINATION bin COMPONENT tests)