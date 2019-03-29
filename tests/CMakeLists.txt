find_package(Boost REQUIRED
        COMPONENTS unit_test_framework
        )

include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/commons
        ${CMAKE_SOURCE_DIR}/formats
        ${Sparsepp_INCLUDE_DIRS}
        ${Qt5Core_INCLUDE_DIRS}
)

file(GLOB SOURCES
        *.cpp
        )

file(GLOB HEADERS
        *.h
        )

file(GLOB UIS *.ui)
file(GLOB QRCS *.qrc)

add_executable(unittests
        ${SOURCES} ${HEADERS}
        ${UIS} ${QRCS}
        )


target_link_libraries(unittests
        commons
        formats
        qtcommons
        Boost::unit_test_framework
        )

install(TARGETS unittests RUNTIME DESTINATION bin COMPONENT tests)