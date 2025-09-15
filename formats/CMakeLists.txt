add_definitions(-DFORMATS_LIBRARY)

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

target_include_directories(formats
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        )

target_link_libraries(formats
        PUBLIC
        Boost::boost
)

# Set target directory
install(TARGETS formats
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        COMPONENT libraries)

#add_subdirectory(test)
