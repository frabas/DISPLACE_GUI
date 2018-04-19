project(formats)

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}
)

find_package(Boost REQUIRED
        COMPONENTS regex filesystem
        )

set(SOURCES
        legacy/ConfigFileReader.cpp legacy/ConfigFileReader.h
        utils/KeyValueReader.cpp utils/KeyValueReader.h
        utils/LineNumberReader.cpp utils/LineNumberReader.h
        utils/prettyprint.h
        legacy/NodesFileReader.h utils/MultifieldReader.h
        )

add_library(formats SHARED
        ${SOURCES}
        )

target_link_libraries(formats
        Boost::regex Boost::filesystem
        )


# Set target directory
install(TARGETS formats DESTINATION lib)

#add_subdirectory(test)
