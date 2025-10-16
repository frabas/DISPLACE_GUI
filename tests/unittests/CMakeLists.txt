find_package(Boost REQUIRED
        COMPONENTS unit_test_framework
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
        ${UIS} ${QRCS} valuespermet_test.cpp)


target_link_libraries(unittests
        commons
        formats
        qtcommons
        Boost::unit_test_framework
        )

target_include_directories(unittests
        PRIVATE
        ${Sparsepp_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/commons
        ${CMAKE_SOURCE_DIR}/formats
)

install(TARGETS unittests RUNTIME DESTINATION bin COMPONENT tests)